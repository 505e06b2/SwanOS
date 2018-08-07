#include "main.h"

duk_ret_t alt_print(duk_context *ctx) {
	if(duk_get_top(ctx) == 0) return 0;
	
	SDL_LockMutex(render);
	render_string = duk_to_string(ctx, 0);
	render_eol = SDL_TRUE;
	SDL_CondWait(render_ready, render);
	SDL_UnlockMutex(render);
	return 0;
}

duk_ret_t alt_getline(duk_context *ctx) { //get user input
	if(duk_get_top(ctx) > 0) {
		SDL_LockMutex(render);
		render_string = duk_to_string(ctx, 0);
		render_eol = SDL_FALSE;
		SDL_CondWait(render_ready, render);
		SDL_UnlockMutex(render);
	}
	
	SDL_LockMutex(ptrs.input);
	input_request = SDL_TRUE;
	SDL_CondWait(ptrs.input_ready, ptrs.input);
	if(close_request) exit(0);
	duk_push_string(ctx, input_buffer);
	SDL_UnlockMutex(ptrs.input);
	return 1;
}

//GUI FUNCS

duk_ret_t gui_clear(duk_context *ctx) { //clear screen
	SDL_LockMutex(render);
	if(duk_get_top(ctx) > 0) {
		unsigned char colour = duk_get_int(ctx, 0);
		if(colour >> 4 != colour & 0x0f) current_colour = colour;
	}
	render_string = "\5\0\5C";
	render_eol = SDL_FALSE;
	SDL_CondWait(render_ready, render);
	SDL_UnlockMutex(render);
}

duk_ret_t gui_blit(duk_context *ctx) { //clear screen
	SDL_LockMutex(render);
	char STRING_BLIT[8] = "\5\0\5B";
	if(duk_get_top(ctx) >= 2) { //get x and y
		*(STRING_BLIT+4) = duk_get_int(ctx, 0);
		*(STRING_BLIT+5) = duk_get_int(ctx, 1);
	} else {
		SDL_UnlockMutex(render);
		duk_push_string(ctx, "SyntaxError: Both coordinates not given");
		return duk_throw(ctx);
	}
	
	unsigned char colour = current_colour;
	if(duk_get_top(ctx) >= 3) { //1-indexed
		colour = duk_get_int(ctx, 2); //0-indexed
		if(colour >> 4 == colour & 0x0f) colour = current_colour;
	}
	
	unsigned char c = ' ';
	if(duk_get_top(ctx) >= 4) { //1-indexed
		c = duk_get_string_default(ctx, 3, " ")[0];
	}
	
	*(STRING_BLIT+6) = colour;
	*(STRING_BLIT+7) = c;
	render_string = STRING_BLIT;
	render_eol = SDL_FALSE;
	SDL_CondWait(render_ready, render);
	SDL_UnlockMutex(render);
}