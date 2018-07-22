#include "main.h"

char *resolve_path(const char *path) { //restrict users into the /computer/ folder
	char *new_memory;
	char *end;
	const char *current;
	
	if(path[0] == '/') {
		new_memory = realloc(CURRENT_PATH, BASE_PATH_SIZE + strlen(path) +1);
		end = new_memory + BASE_PATH_SIZE;
		current = path+1;
	} else {
		new_memory = realloc(CURRENT_PATH, CURRENT_PATH_SIZE + strlen(path) +1);
		end = new_memory + CURRENT_PATH_SIZE;
		while(*end != '/') end--; //removes file from path
		end++;
		current = path;
	}
	
	if(new_memory == NULL) return NULL; //failed to realloc
	CURRENT_PATH = new_memory;
	
	while(*current == '/') current++; //remove slashes at start
	for(; *current; current++) {
		if(*current == '.') {
			if(*(current+1) == '/') { // = "./"
				current++;
				continue;
			} else if(*(current+1) == '.' && *(current+2) == '/') { // this is the "../" case
				current += 2;
				continue;
			}
		} else if(*current == '/' && *(current+1) == '/') {
			continue;
		}
		*end++ = *current;
	}
	*end = '\0';
	//puts(CURRENT_PATH);
	CURRENT_PATH = realloc(CURRENT_PATH, sizeof(char) * (end - CURRENT_PATH )); //shrink to size
	CURRENT_PATH_SIZE = strlen(CURRENT_PATH);
	return CURRENT_PATH;
}

const char *resolve_path_to_js(const char *path) {
	return path + BASE_PATH_SIZE-1; //"remove" prefix path, but keep '/'
}

int main() {
	{ //Scope this so that when we're done, the buffer is removed, etc
		char buffer[1024];
		const char FOLDER_NAME[] = "/computer/";
		getcwd(buffer, sizeof(buffer));
		
		size_t path_size = strlen(buffer);
		CURRENT_PATH = malloc(path_size + sizeof(FOLDER_NAME)); //sizeof(FOLDER_NAME) already has \0, to mark the end of the string
		memcpy(CURRENT_PATH, buffer, path_size);
		memcpy(CURRENT_PATH + path_size, FOLDER_NAME, sizeof(FOLDER_NAME)); //place the folder name at the end (don't need strcat now)
		BASE_PATH_SIZE = strlen(CURRENT_PATH);
		
		char *c = CURRENT_PATH;
		while(*c++) if(*c == '\\') *c = '/'; //it'll never be BASE_PATH[0], so it's fine putting the ++ there
		
		CURRENT_PATH_SIZE = BASE_PATH_SIZE;
	}
	
	duk_context *ctx = duk_create_heap_default();

	duk_idx_t os_object = duk_push_object(ctx);
	duk_push_c_function(ctx, native_puts, DUK_VARARGS);
	duk_put_prop_string(ctx, os_object, "print"); //print: native_puts;
	duk_push_c_function(ctx, native_readfile, DUK_VARARGS);
	duk_put_prop_string(ctx, os_object, "read"); //read: native_readfile;
	duk_push_c_function(ctx, native_stdin, DUK_VARARGS);
	duk_put_prop_string(ctx, os_object, "stdin"); //stdin: native_stdin;
	duk_push_c_function(ctx, native_chdir, DUK_VARARGS);
	duk_put_prop_string(ctx, os_object, "chdir"); //chdir: native_chdir;
	duk_push_c_function(ctx, native_listdir, DUK_VARARGS);
	duk_put_prop_string(ctx, os_object, "list"); //list: native_listdir;
	duk_put_global_string(ctx, "os"); //name the object: `os = {}`
	
	if (duk_peval_string(ctx, "eval(os.read('startup.js'))") != 0) {
		printf("Loading startup.js Failed: %s\n", duk_safe_to_string(ctx, -1));
	}
	duk_pop(ctx);

	duk_destroy_heap(ctx);

	return 0;
}