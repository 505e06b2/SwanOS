#ifdef WIN32

#include <windows.h>
#include <limits.h>

char *realpath(const char *restrict file_name, char *restrict resolved_name) {
	GetFullPathNameA(file_name, MAX_PATH, resolved_name, NULL);
	char *c = resolved_name;
	while(*c) {
		if(*c == '\\') *c = '/';
		c++;
	}
	return resolved_name;
}
#endif