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

typedef struct inputmutex {
	SDL_mutex *lock;
	SDL_cond *ready;
	char buffer[1024];
	SDL_bool request;
	SDL_bool close_request;
} inputmutex;

typedef struct rendermutex {
	SDL_mutex *lock;
	SDL_cond *ready;
	SDL_bool eol;
	const char *string;
	unsigned char colour;
	SDL_Rect cursor_pos;
} rendermutex;

typedef struct Colour { //I'd use the SDL one, but it has RGBA and I don't need alpha
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Colour;

inputmutex input;
rendermutex rendering;
Colour palette[16];

//main.c
void blitChar(const unsigned char, const unsigned char, SDL_Rect *);
void blitString(const unsigned char *, const unsigned char, SDL_Rect *);
void renderFlip();
void renderClear(unsigned char);

//eventloop.c
void eventLoop();

//native.c
duk_ret_t alt_print(duk_context *);
duk_ret_t alt_getline(duk_context *);

duk_ret_t gui_clear(duk_context *);
duk_ret_t gui_blit(duk_context *);
duk_ret_t gui_getcursor(duk_context *);
duk_ret_t gui_setcursor(duk_context *);

#endif