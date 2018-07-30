#ifdef WIN32

#include <windows.h> //to make this file not need to include the main project header
#include <sys/stat.h>

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

#endif
