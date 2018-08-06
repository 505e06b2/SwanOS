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