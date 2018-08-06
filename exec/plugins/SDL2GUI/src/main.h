#ifndef __SDL2GUI_MAIN_H
#define __SDL2GUI_MAIN_H

#include "duktape.h"

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#ifdef WIN32
	#define EXPORT_FUNCTION __declspec(dllexport)
#else
	#define EXPORT_FUNCTION
#endif

typedef struct SDL_Pointers {
	SDL_Window *w;
	SDL_Renderer *r;
	SDL_Texture *t;
	
	SDL_Texture *font;
	
	SDL_mutex *input;
	SDL_cond *input_ready;
} SDL_Pointers;

SDL_mutex *initial;
SDL_cond *initial_ready;
SDL_bool initialised;

//native.c
duk_ret_t alt_print(duk_context *);

#endif