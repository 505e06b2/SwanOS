#include "main.h"

void closeInput() {
	input.close_request = SDL_FALSE;
	blitChar(' ', rendering.colour, &rendering.cursor_pos); //remove cursor
	rendering.cursor_pos.x = 0;
	rendering.cursor_pos.y += FONT_WH;
	SDL_StopTextInput();
	SDL_CondSignal(input.ready);
}

void eventLoop() {
	char run = 1;
	SDL_Event e;
	char *input_char = NULL;
	SDL_bool CTRL_flag = SDL_FALSE;
	
	SDL_StopTextInput();
	while(run) { //here we go
		
		if(input_char == NULL) { //check if request sent
			SDL_LockMutex(input.lock);
			if(input.request) {
				input_char = input.buffer;
				input.request = SDL_FALSE;
				SDL_StartTextInput();
			}
			SDL_UnlockMutex(input.lock);
		}
		
		SDL_LockMutex(rendering.lock); //lock render mutex here, since we use elements from "rendering"
		while(SDL_PollEvent(&e) != 0) { //handle all events
			switch(e.type) {
                case SDL_QUIT:
                    run = 0;
					input.close_request = SDL_TRUE;
					SDL_CondSignal(input.ready); //make sure we don't block
                    break;
				case SDL_WINDOWEVENT:
					switch (e.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							renderFramebuffer();
							break;
					}
					break;
                case SDL_TEXTINPUT:
                    blitChar(e.text.text[0], rendering.colour, &rendering.cursor_pos);
					rendering.cursor_pos.x += FONT_WH;
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
							if(input_char != input.buffer) { //input buffer is the start point
								blitChar(' ', rendering.colour, &rendering.cursor_pos); //remove cursor
								input_char--;
								rendering.cursor_pos.x -= FONT_WH;
								if(rendering.cursor_pos.x < 0) {
									rendering.cursor_pos.y -= FONT_WH;
									rendering.cursor_pos.x = SCREEN_WIDTH - FONT_WH;
								}
								blitChar(' ', rendering.colour, &rendering.cursor_pos);
							}
							
						} else if(CTRL_flag) {
							switch(e.key.keysym.sym) {
								case 'c':
									*input.buffer = '\0'; //make buffer null
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
		
		if(rendering.string != NULL) {
			if(memcmp(rendering.string, "\5\0\5", 3) == 0) {
				switch(rendering.string[3]) {
					case 'C':
						renderClear(rendering.colour);
						rendering.cursor_pos.x = 0;
						rendering.cursor_pos.y = 0;
						break;
					case 'B': 
						{
							SDL_Rect temp = { *((short *)(rendering.string+6)) , *((short *)(rendering.string+8)), FONT_WH,FONT_WH};
							blitChar(rendering.string[5], rendering.string[4], &temp);
						}
						break;
				}
			} else {
				blitString(rendering.string, rendering.colour, &rendering.cursor_pos);
			}
			if(rendering.eol) {
				rendering.cursor_pos.x = 0;
				rendering.cursor_pos.y += FONT_WH;
			}
			rendering.string = NULL;
			rendering.eol = SDL_FALSE;
		}
		SDL_CondSignal(rendering.ready);
		SDL_UnlockMutex(rendering.lock);
		
		if(input_char) {
			blitChar(' ', rendering.colour << 4, &rendering.cursor_pos); //foreground colour is the cursor colour
		}
		
		renderFlip();
	}
}
