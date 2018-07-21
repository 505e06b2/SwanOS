#include "duktape.h"
#include "main.h"

static char *resolve_path(const char *path) { //restrict users into the /computer/ folder
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

static const char *resolve_path_to_js(const char *path) {
	return path + BASE_PATH_SIZE-1; //"remove" prefix path, but keep '/'
}

static duk_ret_t native_puts(duk_context *ctx) { //Just prints lmao
	if(duk_get_top(ctx) == 0) return 0;
	puts(duk_to_string(ctx, 0));
	return 0;
}

static duk_ret_t native_readfile(duk_context *ctx) { //read file and return the contents as a JS String
	if(duk_get_top(ctx) == 0) return 0;
	FILE *f = fopen(resolve_path(duk_to_string(ctx, 0)), "r");
	if(f == NULL) return 0;
	char buffer[1024]; //read in 1k chunks
	size_t chunks = 0;
	size_t received_bytes = 0;
	duk_push_string(ctx, ""); //delimiter
	while(received_bytes = fread(buffer, sizeof(char), sizeof(buffer), f)) {
		duk_push_lstring(ctx, buffer, received_bytes);
		chunks++;
	}
	duk_join(ctx, chunks);
	return 1;
}

static duk_ret_t native_stdin(duk_context *ctx) { //get user input
	char buffer[1024];
	if(duk_get_top(ctx) > 0) {
		printf("%s", duk_to_string(ctx, 0));
		fflush(stdout); //Just in case the output doesn't get flush (like in some IDEs / Terminals)
	}
	char *check = fgets(buffer, sizeof(buffer), stdin);
	if(check) {
		duk_push_lstring(ctx, buffer, strlen(buffer)-1); //remove \n from fgets
		return 1;
	}
	return 0;
}

static duk_ret_t native_chdir(duk_context *ctx) { //change directory
	if(duk_get_top(ctx) > 0) {
		duk_size_t path_length;
		const char *str = duk_get_lstring(ctx, 0, &path_length);
		char *buffer = malloc(path_length +1); //+1 for '/'
		memcpy(buffer, str, path_length);
		*(buffer + path_length) = '/';
		*(buffer + path_length +1) = '\0';
		
		char CURRENT_PATH_BACKUP[CURRENT_PATH_SIZE+1];
		size_t CURRENT_PATH_BACKUP_SIZE = CURRENT_PATH_SIZE+1;
		memcpy(CURRENT_PATH_BACKUP, CURRENT_PATH, CURRENT_PATH_BACKUP_SIZE);
		
		const char *path = resolve_path(buffer);
		free(buffer);
		
		if(chdir(path) == 0) {
			duk_push_string(ctx, resolve_path_to_js(path));
			return 1;
		} else { //if it failed
			memcpy(CURRENT_PATH, CURRENT_PATH_BACKUP, CURRENT_PATH_BACKUP_SIZE);
			CURRENT_PATH_SIZE = CURRENT_PATH_BACKUP_SIZE-1;
		}
	}
	return 0;
}

int main() {
	{ //Scope this so that when we're done, the buffer is removed, etc
		char buffer[1024];
		const char FOLDER_NAME[] = "/computer/";
		getcwd(buffer, sizeof(buffer));
		
		size_t path_size = strlen(buffer);
		BASE_PATH = malloc(path_size + sizeof(FOLDER_NAME)); //sizeof(FOLDER_NAME) already has \0, to mark the end of the string
		memcpy(BASE_PATH, buffer, path_size);
		memcpy(BASE_PATH + path_size, FOLDER_NAME, sizeof(FOLDER_NAME)); //place the folder name at the end (don't need strcat now)
		BASE_PATH_SIZE = strlen(BASE_PATH);
		
		char *c = BASE_PATH;
		while(*c++) if(*c == '\\') *c = '/'; //it'll never be BASE_PATH[0], so it's fine putting the ++ there
		
		CURRENT_PATH = malloc(BASE_PATH_SIZE+1);
		CURRENT_PATH_SIZE = BASE_PATH_SIZE;
		memcpy(CURRENT_PATH, BASE_PATH, BASE_PATH_SIZE+1);
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
	duk_put_global_string(ctx, "os"); //name the object: `os = {}`
	
	if (duk_peval_string(ctx, "eval(os.read('startup.js'))") != 0) {
		printf("Loading startup.js Failed: %s\n", duk_safe_to_string(ctx, -1));
	}
	duk_pop(ctx);

	duk_destroy_heap(ctx);

	return 0;
}