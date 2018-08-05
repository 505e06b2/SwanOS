#include "duktape.h"

#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#ifdef WIN32
	#define EXPORT_FUNCTION __declspec(dllexport)
#else
	#define EXPORT_FUNCTION
#endif

typedef struct SDL_pointers {
	SDL_Window *w;
	SDL_Renderer *r;
} SDL_pointers;

void threadQuit(SDL_pointers *ptrs) {
	SDL_DestroyRenderer(ptrs->r);
	SDL_DestroyWindow(ptrs->w);
	SDL_Quit();
}

int threadStart(void *arg) {
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_pointers ptrs = (struct SDL_pointers){0};
	
	ptrs.w = SDL_CreateWindow("SDL2GUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(ptrs.w == NULL) return -1;
	
	ptrs.r = SDL_CreateRenderer(ptrs.w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if(ptrs.r == NULL) return -1;
	
	SDL_Delay(2000);
	threadQuit(&ptrs);
	return 0;
}

EXPORT_FUNCTION void init(duk_context *ctx) {
	SDL_Thread *thread = SDL_CreateThread(threadStart, "guithread", (void *)NULL);
}
 