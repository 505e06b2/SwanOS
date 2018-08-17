#include "main.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *buffer = NULL;
SDL_Texture *font = NULL;


void blitChar(const unsigned char c, const unsigned char colours, SDL_Rect *dest) {
	if(c == '\n') {
		dest->y += FONT_WH;
		dest->x = -1;
		return;
	}
	
	SDL_Rect src;
	//calculate ascii charmap
	src = (SDL_Rect){
		(c % (16)) * FONT_WH, //X
		(c / (16)) * FONT_WH, //Y
		FONT_WH, FONT_WH //W/H
	};
	
	if(dest->x >= SCREEN_WIDTH) {
		dest->x = 0;
		dest->y += FONT_WH;
	}
	
	if(dest->y >= SCREEN_HEIGHT) {
		dest->y = SCREEN_HEIGHT - FONT_WH;
		SDL_Rect src = {0, FONT_WH, SCREEN_WIDTH, SCREEN_HEIGHT};
		SDL_Rect dest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - FONT_WH};
		SDL_RenderCopy(renderer, buffer, &src, &dest);
		dest = (SDL_Rect){0, SCREEN_HEIGHT - FONT_WH, SCREEN_WIDTH, FONT_WH};
		SDL_RenderFillRect(renderer, &dest);
	}
	
	{ //BG
		const unsigned char bgcolour = colours >> 4;
		SDL_SetRenderDrawColor(renderer, palette[bgcolour].r, palette[bgcolour].g, palette[bgcolour].b, 0xff);
		SDL_RenderFillRect(renderer, dest);
	}

	{ //FG
		const unsigned char fgcolour = colours & 0x0f;
		SDL_SetTextureColorMod(font, palette[fgcolour].r, palette[fgcolour].g, palette[fgcolour].b);
		SDL_RenderCopy(renderer, font, &src, dest);
	}
}

void blitString(const unsigned char *str, const unsigned char colours, SDL_Rect *dest) {
	for(const unsigned char *current = str; *current; *current++) {
		blitChar(*current, colours, dest);
		dest->x += FONT_WH;
	}
}

void renderClear(unsigned char colour) {
	colour = colour >> 4;
	SDL_SetRenderDrawColor(renderer, palette[colour].r, palette[colour].g, palette[colour].b, 0xff);
	SDL_RenderClear(renderer);
}

void renderFlip() {
	SDL_SetRenderTarget(renderer, NULL); //set renderer back to window
	SDL_RenderCopy(renderer, buffer, NULL, NULL); //stretch to window
	SDL_RenderPresent(renderer); //vsync should keep this from being too quick, but I should implement an fps cap too
	SDL_SetRenderTarget(renderer, buffer); //set renderer to texture
}

void threadQuit() {
	SDL_DestroyCond(rendering.ready);
	rendering.ready = NULL;
	SDL_DestroyMutex(rendering.lock);
	
	SDL_DestroyCond(input.ready);
	input.ready = NULL;
	SDL_DestroyMutex(input.lock);
	
	SDL_DestroyTexture(font);
	SDL_DestroyTexture(buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int threadStart(void *arg) {
	SDL_LockMutex(rendering.lock);
	
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	
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
			(window = SDL_CreateWindow("SDL2GUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SDL_WINDOW_SHOWN)) && //increase window size by 2, so it's not so small...
			(renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)) &&
			(buffer = SDL_CreateTexture(renderer, PIXEL_FORMAT, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT)) &&
			(tempfont = SDL_LoadBMP(charsetpath)) && //load font
			(SDL_SetColorKey(tempfont, SDL_TRUE, SDL_MapRGB(tempfont->format, 0, 0, 0) ) == 0) && //remove bg from font file
			(font = SDL_CreateTextureFromSurface(renderer, tempfont)) //convert font to texture
		)) { //error if failed
			printf(">> SDL Error: %s\n", SDL_GetError());
			threadQuit();
		
			rendering.eol = SDL_FALSE;
			SDL_CondSignal(rendering.ready); //must signal, or the main thread is locked forever
			SDL_UnlockMutex(rendering.lock);
			return -1;
		}
		
		SDL_FreeSurface(tempfont); //clean up
	}
	
	//Shared vars
	rendering.eol = SDL_TRUE;
	input.request = SDL_FALSE;
	input.close_request = SDL_TRUE;
	
	//set up colour palette
	palette[0x0] = (Colour){0x28, 0x28, 0x28}; //bg
	palette[0x1] = (Colour){0xcc, 0x24, 0x1d}; //red
	palette[0x2] = (Colour){0x98, 0x97, 0x1a}; //green
	palette[0x3] = (Colour){0xd7, 0x99, 0x23}; //yellow
	palette[0x4] = (Colour){0x45, 0x85, 0x88}; //blue
	palette[0x5] = (Colour){0xb1, 0x62, 0x86}; //purple
	palette[0x6] = (Colour){0x68, 0x9d, 0x6a}; //aqua
	palette[0x7] = (Colour){0xa8, 0x99, 0x84}; //grey
	palette[0x8] = (Colour){0x92, 0x83, 0x74}; //grey
	palette[0x9] = (Colour){0xfb, 0x49, 0x34}; //red
	palette[0xa] = (Colour){0xb8, 0xbb, 0x26}; //green
	palette[0xb] = (Colour){0xfa, 0xbd, 0x2f}; //yellow
	palette[0xc] = (Colour){0x83, 0xa5, 0x98}; //blue
	palette[0xd] = (Colour){0xd3, 0x86, 0x9b}; //purple
	palette[0xe] = (Colour){0x8e, 0xc0, 0x7c}; //aqua
	palette[0xf] = (Colour){0xeb, 0xdb, 0xb2}; //fg
	
	rendering.colour = 0x07;
	rendering.cursor_pos = (SDL_Rect){0,0, FONT_WH,FONT_WH};
	
	SDL_SetRenderTarget(renderer, buffer); //set renderer to texture
	renderClear(rendering.colour);
	
	SDL_CondSignal(rendering.ready);
	SDL_UnlockMutex(rendering.lock); //unlock here, because once it's signalled, the main thread mutex has to wait on the lock
	
	eventLoop();
	
	threadQuit();
	return 0;
}

EXPORT_FUNCTION void init(duk_context *ctx, const char *path) { //path to this dll
	//initialise mutexes
	if( (rendering.lock = SDL_CreateMutex()) == NULL || (rendering.ready = SDL_CreateCond()) == NULL ) return;
	if( (input.lock = SDL_CreateMutex()) == NULL || (input.ready = SDL_CreateCond()) == NULL ) return;
	
	//Wait on other thread to initialise
	SDL_LockMutex(rendering.lock);
	SDL_Thread *thread = SDL_CreateThread(threadStart, "guithread", (void *)path); //put it here, so we don't hit a mutex infinite lock
	SDL_CondWait(rendering.ready, rendering.lock); //wait on sdl2 initialising
	
	if(rendering.eol) { //add JS hooks to the global namespace
		duk_bool_t ret = duk_get_global_string(ctx, "os"); //make "os" become -1
		if(ret == 0) return; //no global "os" somehow
		
		duk_idx_t guiobj = duk_push_object(ctx);
		puts(">> Added [obj] os.gui");
		
			duk_push_c_function(ctx, gui_clear, DUK_VARARGS);
			duk_put_prop_string(ctx, guiobj, "clear");
			puts(">> Added [func] os.gui.clear");
		
			duk_push_c_function(ctx, gui_blit, DUK_VARARGS);
			duk_put_prop_string(ctx, guiobj, "blit");
			puts(">> Added [func] os.gui.blit");
		
			duk_idx_t cursorobj = duk_push_object(ctx);
			puts(">> Added [obj] os.gui.cursor");
		
				duk_push_c_function(ctx, gui_getcursor, DUK_VARARGS);
				duk_put_prop_string(ctx, cursorobj, "get");
				puts(">> Added [func] os.gui.cursor.get");
				
				duk_push_c_function(ctx, gui_setcursor, DUK_VARARGS);
				duk_put_prop_string(ctx, cursorobj, "set");
				puts(">> Added [func] os.gui.cursor.set");
		
			duk_put_prop_string(ctx, guiobj, "cursor");
		
		duk_put_prop_string(ctx, -2, "gui"); //guiobj is currently -1, "os" is -2
		
		duk_push_c_function(ctx, alt_print, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "print");
		puts(">> Replaced [func] os.print");
		
		duk_push_c_function(ctx, alt_getline, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "getline");
		puts(">> Replaced [func] os.getline");
	
		duk_pop(ctx); //remove dupe "os", as a reference was brought to the front of the stack
	}
	
	SDL_UnlockMutex(rendering.lock);
}
 