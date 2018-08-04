#ifndef __MAIN_H_
#define __MAIN_H_

#include <unistd.h>
#include <dirent.h> //listing directory
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

#include "duktape.h"

#define ENABLED_PLUGINS_TXT "plugins/enabled.txt"

size_t BASE_PATH_SIZE;
char *CURRENT_PATH;
size_t CURRENT_PATH_SIZE;

//main.c
char *resolve_path(const char *);
const char *resolve_path_to_js(const char *);

//native.c
duk_ret_t native_puts(duk_context *);
duk_ret_t native_sleep(duk_context *);

duk_ret_t native_runfile(duk_context *);
duk_ret_t native_sandbox(duk_context *);

duk_ret_t native_readfile(duk_context *);
duk_ret_t native_writefile(duk_context *);
duk_ret_t native_removefile(duk_context *);
duk_ret_t native_exists(duk_context *);

duk_ret_t native_getline(duk_context *);
duk_ret_t native_getchar(duk_context *);

duk_ret_t native_mkdir(duk_context *);
duk_ret_t native_rmdir(duk_context *);
duk_ret_t native_chdir(duk_context *);
duk_ret_t native_listdir(duk_context *);

//OS specific: unix.c/windows.c
int system_getchar();
void system_sleep(unsigned int);
void system_loadlib(duk_context *, const char *);

//windows.c
#ifdef WIN32

#define mkdir(A, B) mkdir(A) //emulate posix
char *realpath(const char *restrict, char *restrict);

#endif

#endif