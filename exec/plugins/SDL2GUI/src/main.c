#include "main.h"

void renderFlip(SDL_Pointers *ptrs) {
	SDL_SetRenderTarget(ptrs->r, NULL); //set renderer back to window
	SDL_RenderCopy(ptrs->r, ptrs->t, NULL, NULL); //stretch to window
	SDL_RenderPresent(ptrs->r); //vsync should keep this from being too quick, but I should implement an fps cap too
	SDL_SetRenderTarget(ptrs->r, ptrs->t); //set renderer to texture
}

void eventLoop(SDL_Pointers *ptrs) {
	char run = 1;
	SDL_Event e;
	SDL_SetRenderTarget(ptrs->r, ptrs->t); //set renderer to texture
	
	while(run) { //here we go
		while(SDL_PollEvent(&e) != 0) { //handle all events
			if(e.type == SDL_QUIT) {
				run = 0;
			}
		}
		renderFlip(ptrs);
	}
}

void threadQuit(SDL_Pointers *ptrs) {
	SDL_DestroyCond(ptrs->input_ready);
	ptrs->input_ready = NULL;
	SDL_UnlockMutex(ptrs->input); //Force unlock
	SDL_DestroyMutex(ptrs->input);
	
	SDL_DestroyTexture(ptrs->t);
	SDL_DestroyRenderer(ptrs->r);
	SDL_DestroyWindow(ptrs->w);
	SDL_Quit();
}

int threadStart(void *arg) {
	SDL_LockMutex(initial);
	
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_Pointers ptrs;
	memset(&ptrs, 0, sizeof(SDL_Pointers)); //Really want to make sure these are NULL; not leaving it up to the compiler
	
	{ //scope this as otherwise it'll be here until SDL closes
		SDL_Surface *tempfont;
		
		const char *path = (const char *)arg;
		const char *pathpoint = path + strlen(path);
		while(*pathpoint != '/') pathpoint--;
		const char file[] = "/charmap.bmp";
		size_t pathlen = pathpoint - path;
		char charsetpath[pathlen + sizeof(file)];
		memcpy(charsetpath, path, pathlen);
		memcpy(charsetpath + pathlen, file, sizeof(file));
		
	
		if(!( //if not everything initialised, return -1
			(ptrs.w = SDL_CreateWindow("SDL2GUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SDL_WINDOW_SHOWN)) && //increase window size by 2, so it's not so small...
			(ptrs.r = SDL_CreateRenderer(ptrs.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)) &&
			(ptrs.t = SDL_CreateTexture(ptrs.r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT)) &&
			(ptrs.input = SDL_CreateMutex()) &&
			(ptrs.input_ready = SDL_CreateCond()) &&
			(tempfont = SDL_LoadBMP(charsetpath)) && //load font
			(ptrs.font = SDL_CreateTextureFromSurface(ptrs.r, tempfont)) //convert font to texture
		)) { //error if failed
			printf(">> SDL Error: %s\n", SDL_GetError());
			threadQuit(&ptrs);
		
			initialised = SDL_FALSE;
			SDL_CondSignal(initial_ready); //must signal, or the main thread is locked forever
			SDL_UnlockMutex(initial);
			return -1;
		}
	}
	
	initialised = SDL_TRUE;
	SDL_CondSignal(initial_ready);
	SDL_UnlockMutex(initial); //unlock here, because once it's signalled, the mutex has to wait on the lock
	
	eventLoop(&ptrs);
	
	threadQuit(&ptrs);
	return 0;
}

EXPORT_FUNCTION void init(duk_context *ctx, const char *path) { //path to this dll
	if( (initial = SDL_CreateMutex()) == NULL || (initial_ready = SDL_CreateCond()) == NULL ) return;	
	
	//Wait on other thread to initialise
	SDL_LockMutex(initial);
	SDL_Thread *thread = SDL_CreateThread(threadStart, "guithread", (void *)path); //put it here, so we don't hit a mutex infinite lock
	SDL_CondWait(initial_ready, initial); //wait on sdl2 initialising
    SDL_UnlockMutex(initial);
	
	SDL_DestroyCond(initial_ready);
	SDL_DestroyMutex(initial);
	initial_ready = NULL;
	initial = NULL;
	
	if(initialised) { //add JS hooks to the global namespace
		duk_bool_t ret = duk_get_global_string(ctx, "os"); //make "os" become -1
		if(ret == 0) return; //no global "os" somehow
		
		duk_push_boolean(ctx, 1);
		duk_put_prop_string(ctx, -2, "SDL2GUI"); //bool is currently -1, "os" is -2
		puts(">> Added [bool] os.SDL2GUI");
		
		duk_push_c_function(ctx, alt_print, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "print");
		puts(">> Replaced [func] os.print");
	
		duk_pop(ctx); //remove dupe "os", as a reference was brought to the front of the stack
	}
}
 