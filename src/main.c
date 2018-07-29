#include "main.h"

char *resolve_path(const char *path) { //restrict users into the /computer/ folder
	char *new_memory;
	char *end;
	
	if(path[0] == '/') {
		new_memory = realloc(CURRENT_PATH, BASE_PATH_SIZE + strlen(path) +1);
		end = new_memory + BASE_PATH_SIZE;
	} else {
		new_memory = realloc(CURRENT_PATH, CURRENT_PATH_SIZE + strlen(path) +1);
		end = new_memory + CURRENT_PATH_SIZE;
		while(*end != '/') end--; //removes file from path
		end++;
	}
	
	if(new_memory == NULL) return NULL; //failed to realloc
	CURRENT_PATH = new_memory;
	
	//backup current path
	char CURRENT_PATH_BACKUP[CURRENT_PATH_SIZE+1];
	size_t CURRENT_PATH_BACKUP_SIZE = CURRENT_PATH_SIZE+1;
	memcpy(CURRENT_PATH_BACKUP, CURRENT_PATH, CURRENT_PATH_BACKUP_SIZE);
	
	memcpy(end, path, strlen(path)+1); //add new path to current one
	
	char buffer[MAX_PATH];
	realpath(CURRENT_PATH, buffer); //convert to real path with no ".."
	
	if(strncmp(buffer, CURRENT_PATH, BASE_PATH_SIZE) == 0) { //if the base path is disturbed, then use the backup
		CURRENT_PATH_SIZE = strlen(buffer);
		memcpy(CURRENT_PATH, buffer, CURRENT_PATH_SIZE+1);
	} else {
		memcpy(CURRENT_PATH, CURRENT_PATH_BACKUP, CURRENT_PATH_BACKUP_SIZE);
		CURRENT_PATH_SIZE = CURRENT_PATH_BACKUP_SIZE;
	}
	
	//puts(CURRENT_PATH);
	
	CURRENT_PATH = realloc(CURRENT_PATH, CURRENT_PATH_SIZE+1); //shrink to size
	return CURRENT_PATH;
}

const char *resolve_path_to_js(const char *path) {
	return path + BASE_PATH_SIZE-1; //"remove" prefix path, but keep '/'
}

typedef struct native_to_js { //for the loop
	duk_ret_t (*native)(duk_context *);
	const char *js;
} native_to_js;

int main() {
	{ //Scope this so that when we're done, the buffer is removed, etc
		char buffer[1024];
		const char FOLDER_NAME[] = "/computer"; // <- DO NOT PUT A SLASH ON THE END
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
	
	//Create heap
	duk_context *ctx = duk_create_heap_default();
	
	//Create object
	duk_idx_t os_object = duk_push_object(ctx);
	
	{ //scope this loop, so we can get rid of it after we need it
		native_to_js all_functions[] = {
			{native_puts, "print"},
			{native_runfile, "run"},
			{native_sandbox, "sandbox"},
			{native_readfile, "read"},
			{native_writefile, "write"},
			{native_removefile, "remove"},
			{native_stdin, "stdin"},
			{native_mkdir, "mkdir"},
			{native_chdir, "chdir"},
			{native_rmdir, "rmdir"},
			{native_listdir, "listdir"},
			{native_exists, "exists"}
		};
		
		for(size_t i = 0, e = sizeof(all_functions) / sizeof(native_to_js); i < e; i++) { //we need to divide so we can get the length, and not just bytes
			duk_push_c_function(ctx, all_functions[i].native, DUK_VARARGS);
			duk_put_prop_string(ctx, os_object, all_functions[i].js);
		}
	}
	
	duk_put_global_string(ctx, "os"); //name the object: `os = {}`
	
	//Run script
	duk_peval_string(ctx, "os.run('/startup.js')");
	puts(duk_safe_to_string(ctx, -1));
	duk_pop(ctx);

	duk_destroy_heap(ctx);

	return 0;
}