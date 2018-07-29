#ifdef WIN32

#include <windows.h> //to make this file not need to include the main project header

char *realpath(const char *restrict file_name, char *restrict resolved_name) {
	GetFullPathNameA(file_name, PATH_MAX, resolved_name, NULL);
	char *c = resolved_name;
	while(*c) {
		if(*c == '\\') *c = '/';
		c++;
	}
	return resolved_name;
}

#endif
