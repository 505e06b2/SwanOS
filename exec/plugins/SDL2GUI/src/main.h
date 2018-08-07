#ifndef __SDL2GUI_MAIN_H
#define __SDL2GUI_MAIN_H

#include "duktape.h"

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define FONT_WH 8 //font width and height (square); this is the grid size in px

#define SCREEN_COLS SCREEN_WIDTH / FONT_WH
#define SCREEN_ROWS SCREEN_HEIGHT / FONT_WH

#define PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888

#ifdef WIN32
	#define EXPORT_FUNCTION __declspec(dllexport)
#else
	#define EXPORT_FUNCTION
#endif

typedef struct SDL_Pointers {
	SDL_Window *w;
	SDL_Renderer *r;
	SDL_Texture *buffer;
	
	SDL_Texture *font;
	
	SDL_mutex *input;
	SDL_cond *input_ready;
} SDL_Pointers;

typedef struct Colour { //I'd use the SDL one, but it has RGB
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Colour;

Colour palette[16];
unsigned char current_colour;

//these also have uses in the thread_start()
SDL_mutex *render; //init: main function lock
SDL_cond *render_ready; //init: return to main function
SDL_bool render_eol; //init: checks if successful
const char *render_string;

char input_buffer[1024];
SDL_bool input_request;
SDL_bool close_request;

SDL_Pointers ptrs; //this will literally always be in memory anyways, but I need it available to duktape...
SDL_Rect cursor_pos;

void blitChar(const unsigned char, const unsigned char colours, SDL_Rect *);
void blitString(const unsigned char *, const unsigned char colours, SDL_Rect *);

//native.c
duk_ret_t alt_print(duk_context *);
duk_ret_t alt_getline(duk_context *);

duk_ret_t gui_clear(duk_context *);
duk_ret_t gui_blit(duk_context *);

#endif