#include "main.h"

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
		SDL_RenderCopy(ptrs.r, ptrs.buffer, &src, &dest);
		dest = (SDL_Rect){0, SCREEN_HEIGHT - FONT_WH, SCREEN_WIDTH, FONT_WH};
		SDL_RenderFillRect(ptrs.r, &dest);
	}
	
	{ //BG
		const unsigned char bgcolour = colours >> 4;
		SDL_SetRenderDrawColor(ptrs.r, palette[bgcolour].r, palette[bgcolour].g, palette[bgcolour].b, 0xff);
		SDL_RenderFillRect(ptrs.r, dest);
	}

	{ //FG
		const unsigned char fgcolour = colours & 0x0f;
		SDL_SetTextureColorMod(ptrs.font, palette[fgcolour].r, palette[fgcolour].g, palette[fgcolour].b);
		SDL_RenderCopy(ptrs.r, ptrs.font, &src, dest);
	}
}

void blitString(const unsigned char *str, const unsigned char colours, SDL_Rect *dest) {
	for(const unsigned char *current = str; *current; *current++) {
		blitChar(*current, colours, dest);
		dest->x += FONT_WH;
	}
}

void renderFlip() {
	SDL_SetRenderTarget(ptrs.r, NULL); //set renderer back to window
	SDL_RenderCopy(ptrs.r, ptrs.buffer, NULL, NULL); //stretch to window
	SDL_RenderPresent(ptrs.r); //vsync should keep this from being too quick, but I should implement an fps cap too
	SDL_SetRenderTarget(ptrs.r, ptrs.buffer); //set renderer to texture
}

void closeInput() {
	close_request = SDL_FALSE;
	blitChar(' ', current_colour, &cursor_pos); //remove cursor
	cursor_pos.x = 0;
	cursor_pos.y += FONT_WH;
	SDL_StopTextInput();
	SDL_CondSignal(ptrs.input_ready);
}

void eventLoop() {
	char run = 1;
	SDL_Event e;
	char *input_char = NULL;
	SDL_bool CTRL_flag = SDL_FALSE;
	
	SDL_StopTextInput();
	while(run) { //here we go
		
		if(input_char == NULL) { //check if request sent
			SDL_LockMutex(ptrs.input);
			if(input_request) {
				input_char = input_buffer;
				input_request = SDL_FALSE;
				SDL_StartTextInput();
			}
			SDL_UnlockMutex(ptrs.input);
		}
		
		SDL_LockMutex(render); //lock render mutex here, since we use current_colour
		while(SDL_PollEvent(&e) != 0) { //handle all events
			switch(e.type) {
                case SDL_QUIT:
                    run = 0;
					close_request = SDL_TRUE;
					SDL_CondSignal(ptrs.input_ready); //make sure we don't block
                    break;
                case SDL_TEXTINPUT:
                    blitChar(e.text.text[0], current_colour, &cursor_pos);
					cursor_pos.x += FONT_WH;
					*input_char = e.text.text[0];
					input_char++;
                    break;
				case SDL_KEYDOWN:
					if(input_char) {
						if(e.key.keysym.sym == SDLK_RETURN) {
							*input_char = '\0';
							input_char = NULL;
							closeInput();
							
						} else if(e.key.keysym.sym == SDLK_BACKSPACE) {
							if(input_char != input_buffer) { //input buffer is the start point
								blitChar(' ', current_colour, &cursor_pos); //remove cursor
								input_char--;
								cursor_pos.x -= FONT_WH;
								if(cursor_pos.x < 0) {
									cursor_pos.y -= FONT_WH;
									cursor_pos.x = SCREEN_WIDTH - FONT_WH;
								}
								blitChar(' ', current_colour, &cursor_pos);
							}
							
						} else if(CTRL_flag) {
							switch(e.key.keysym.sym) {
								case 'c':
									*input_buffer = '\0'; //make buffer null
									input_char = NULL;
									closeInput();
									break;
							}
						}
					}
					
					if(e.key.keysym.sym == SDLK_LCTRL) CTRL_flag = SDL_TRUE;
					break;
					
				case SDL_KEYUP:
					if(e.key.keysym.sym == SDLK_LCTRL) CTRL_flag = SDL_FALSE;
					break;
            }
		}
		
		if(render_string != NULL) {
			if(memcmp(render_string, "\5\0\5", 3) == 0) {
				switch(render_string[3]) {
					case 'C':
						{
							unsigned char bg = current_colour >> 4;
							SDL_SetRenderDrawColor(ptrs.r, palette[bg].r, palette[bg].g, palette[bg].b, 0xff);
						}
						SDL_RenderClear(ptrs.r);
						cursor_pos.x = 0;
						cursor_pos.y = 0;
						break;
					case 'B':
						{
							SDL_Rect temprect = {render_string[4] * FONT_WH, render_string[5] * FONT_WH, FONT_WH,FONT_WH};
							blitChar(render_string[7], render_string[6], &temprect);
						}
						break;
				}
			} else {
				blitString(render_string, current_colour, &cursor_pos);
			}
			if(render_eol) {
				cursor_pos.x = 0;
				cursor_pos.y += FONT_WH;
			}
			render_string = NULL;
			render_eol = SDL_FALSE;
		}
		SDL_CondSignal(render_ready);
		SDL_UnlockMutex(render);
		
		if(input_char) {
			blitChar(' ', current_colour << 4, &cursor_pos);
		}
		
		renderFlip();
	}
}

void threadQuit() {
	SDL_DestroyCond(render_ready);
	render_ready = NULL;
	SDL_DestroyMutex(render);
	
	SDL_DestroyCond(ptrs.input_ready);
	ptrs.input_ready = NULL;
	SDL_DestroyMutex(ptrs.input);
	
	SDL_DestroyTexture(ptrs.buffer);
	SDL_DestroyRenderer(ptrs.r);
	SDL_DestroyWindow(ptrs.w);
	SDL_Quit();
}

int threadStart(void *arg) {
	SDL_LockMutex(render);
	
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	
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
			(ptrs.buffer = SDL_CreateTexture(ptrs.r, PIXEL_FORMAT, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT)) &&
			(ptrs.input = SDL_CreateMutex()) &&
			(ptrs.input_ready = SDL_CreateCond()) &&
			(tempfont = SDL_LoadBMP(charsetpath)) && //load font
			(SDL_SetColorKey(tempfont, SDL_TRUE, SDL_MapRGB(tempfont->format, 0, 0, 0) ) == 0) && //remove bg from font file
			(ptrs.font = SDL_CreateTextureFromSurface(ptrs.r, tempfont)) //convert font to texture
		)) { //error if failed
			printf(">> SDL Error: %s\n", SDL_GetError());
			threadQuit();
		
			render_eol = SDL_FALSE;
			SDL_CondSignal(render_ready); //must signal, or the main thread is locked forever
			SDL_UnlockMutex(render);
			return -1;
		}
		
		SDL_FreeSurface(tempfont); //clean up
	}
	
	//Shared vars
	render_eol = SDL_TRUE;
	input_request = SDL_FALSE;
	close_request = SDL_TRUE;
	
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
	current_colour = 0x07;
	
	cursor_pos = (SDL_Rect){0,0, FONT_WH,FONT_WH};
	
	SDL_SetRenderTarget(ptrs.r, ptrs.buffer); //set renderer to texture
	SDL_SetRenderDrawColor(ptrs.r, palette[0].r, palette[0].g, palette[0].b, 0xff);
	SDL_RenderClear(ptrs.r);
	
	SDL_CondSignal(render_ready);
	SDL_UnlockMutex(render); //unlock here, because once it's signalled, the main thread mutex has to wait on the lock
	
	eventLoop();
	
	threadQuit();
	return 0;
}

EXPORT_FUNCTION void init(duk_context *ctx, const char *path) { //path to this dll
	if( (render = SDL_CreateMutex()) == NULL || (render_ready = SDL_CreateCond()) == NULL ) return;	
	
	//Wait on other thread to initialise
	SDL_LockMutex(render); //we re-use render later, in the event loop
	SDL_Thread *thread = SDL_CreateThread(threadStart, "guithread", (void *)path); //put it here, so we don't hit a mutex infinite lock
	SDL_CondWait(render_ready, render); //wait on sdl2 initialising
    SDL_UnlockMutex(render);
	
	if(render_eol) { //add JS hooks to the global namespace
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
		
		duk_put_prop_string(ctx, -2, "gui"); //guiobj is currently -1, "os" is -2
		
		duk_push_c_function(ctx, alt_print, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "print");
		puts(">> Replaced [func] os.print");
		
		duk_push_c_function(ctx, alt_getline, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "getline");
		puts(">> Replaced [func] os.getline");
	
		duk_pop(ctx); //remove dupe "os", as a reference was brought to the front of the stack
	}
}
 