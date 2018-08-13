#include "main.h"

duk_ret_t alt_print(duk_context *ctx) {
	if(duk_get_top(ctx) == 0) return 0;
	
	SDL_LockMutex(rendering.lock);
	rendering.string = duk_to_string(ctx, 0);
	rendering.eol = SDL_TRUE;
	SDL_CondWait(rendering.ready, rendering.lock);
	SDL_UnlockMutex(rendering.lock);
	return 0;
}

duk_ret_t alt_getline(duk_context *ctx) { //get user input
	if(duk_get_top(ctx) > 0) {
		SDL_LockMutex(rendering.lock);
		rendering.string = duk_to_string(ctx, 0);
		rendering.eol = SDL_FALSE;
		SDL_CondWait(rendering.ready, rendering.lock);
		SDL_UnlockMutex(rendering.lock);
	}
	
	SDL_LockMutex(input.lock);
	input.request = SDL_TRUE;
	SDL_CondWait(input.ready, input.lock);
	if(input.close_request) exit(0);
	duk_push_string(ctx, input.buffer);
	SDL_UnlockMutex(input.lock);
	return 1;
}

//GUI FUNCS

duk_ret_t gui_clear(duk_context *ctx) { //clear screen
	SDL_LockMutex(rendering.lock);
	if(duk_get_top(ctx) > 0) {
		unsigned char colour = duk_get_int(ctx, 0);
		if(colour >> 4 != colour & 0x0f) rendering.colour = colour;
	}
	rendering.string = "\5\0\5C";
	rendering.eol = SDL_FALSE;
	SDL_CondWait(rendering.ready, rendering.lock);
	SDL_UnlockMutex(rendering.lock);
}

duk_ret_t gui_blit(duk_context *ctx) { //clear screen
	SDL_LockMutex(rendering.lock);
	char STRING_BLIT[6 + sizeof(SDL_Rect)] = "\5\0\5B";
	
	SDL_Rect temp = {0, 0, FONT_WH,FONT_WH};
	if(duk_get_top(ctx) >= 2) { //get x and y
		temp.x = duk_get_int(ctx, 0) * FONT_WH;
		temp.y = duk_get_int(ctx, 1) * FONT_WH;
	} else {
		SDL_UnlockMutex(rendering.lock);
		duk_push_string(ctx, "SyntaxError: Both coordinates not given");
		return duk_throw(ctx);
	}
	
	unsigned char colour = rendering.colour;
	if(duk_get_top(ctx) >= 3) { //1-indexed
		colour = duk_get_int(ctx, 2); //0-indexed
		if(colour >> 4 == colour & 0x0f) colour = rendering.colour;
	}
	
	unsigned char c = ' ';
	if(duk_get_top(ctx) >= 4) { //1-indexed
		c = duk_get_string_default(ctx, 3, " ")[0];
	}
	
	*(STRING_BLIT+4) = colour;
	*(STRING_BLIT+5) = c;
	memcpy(STRING_BLIT+6, &temp, sizeof(SDL_Rect));
	rendering.string = STRING_BLIT; //this is fine, since it won't go out of scope until this func ends
	rendering.eol = SDL_FALSE;
	SDL_CondWait(rendering.ready, rendering.lock);
	SDL_UnlockMutex(rendering.lock);
}