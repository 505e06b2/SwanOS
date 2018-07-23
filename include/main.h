#ifndef __MAIN_H_
#define __MAIN_H_

#include <unistd.h>
#include <dirent.h> //listing directory
#include <stdlib.h>
#include <limits.h>

#include "duktape.h"

size_t BASE_PATH_SIZE;
char *CURRENT_PATH;
size_t CURRENT_PATH_SIZE;

//main.c
char *resolve_path(const char *);
const char *resolve_path_to_js(const char *);

//native.c
duk_ret_t native_puts(duk_context *);
duk_ret_t native_listdir(duk_context *);
duk_ret_t native_readfile(duk_context *);
duk_ret_t native_stdin(duk_context *);
duk_ret_t native_chdir(duk_context *);

//windows.c
#ifdef WIN32
#define MAX_PATH _MAX_PATH
char *realpath(const char *restrict, char *restrict);
#endif

#endif