#include "main.h"
#include <windows.h> //to make this file not need to include the main project header
#include <conio.h> //getch

char *realpath(const char *restrict file_name, char *restrict resolved_name) {
	GetFullPathNameA(file_name, PATH_MAX, resolved_name, NULL);
	
	struct stat properties;
    if(stat(resolved_name, &properties) != 0) return NULL; //Path doesn't exist
	
	char *c = resolved_name;
	while(*c) {
		if(*c == '\\') *c = '/';
		c++;
	}
	if(*(c-1) == '/') *(c-1) = '\0';
	return resolved_name;
}

int system_getchar() {
	return getch();
}

void system_sleep(unsigned int milliseconds) {
	Sleep(milliseconds);
}

void system_loadlib(duk_context *ctx, const char *path) {
	HMODULE lib = LoadLibrary(path);
	if(lib == NULL) {
		printf(">> ERROR: Couldn't load '%s'\n", path);
		return;
	}
	FARPROC findfunc = GetProcAddress(lib, "init");
	if(findfunc == NULL) {
		printf(">> ERROR: No 'init' function in '%s'\n>> (Your function may need to be prefixed with '__declspec(dllexport)'", path);
		return;
	}
	(void (*)(duk_context *))findfunc(ctx);
}