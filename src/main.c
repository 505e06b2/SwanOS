#include "main.h"

char *resolve_path(const char *path) { //restrict users into the /computer folder
	
	if(path[0] != '/') {
		printf("[%s] EVERY PATH SENT TO A NATIVE MUST BE ABSOLUTE => %s\n", (CURRENT_PATH + BASE_PATH_SIZE), path); //Show file currently running, and the path which it doesn't like
		return NULL;
	}
	
	char *new_memory = realloc(CURRENT_PATH, BASE_PATH_SIZE + strlen(path) +1);
	if(new_memory == NULL) return NULL; //failed to realloc
	
	CURRENT_PATH = new_memory;
	char *end = new_memory + BASE_PATH_SIZE;
	
	memcpy(end, path, strlen(path)+1); //add new path to current one
	
	char buffer[PATH_MAX+1];
	char *successful = realpath(CURRENT_PATH, buffer); //convert to real path with no ".."
	
	if(successful && strncmp(buffer, CURRENT_PATH, BASE_PATH_SIZE) == 0) { //if the base path is disturbed, then fail
		CURRENT_PATH_SIZE = strlen(buffer);
		memcpy(CURRENT_PATH, buffer, CURRENT_PATH_SIZE+1);
	} else {
		CURRENT_PATH_SIZE = 0; //Don't use it.
		return NULL;
	}
	
	//puts(CURRENT_PATH);
	
	CURRENT_PATH = realloc(CURRENT_PATH, CURRENT_PATH_SIZE+1); //shrink to size
	return CURRENT_PATH;
}

const char *resolve_path_to_js(const char *path) {
	const char *returnval = path + BASE_PATH_SIZE;
	return (returnval[0] == '\0') ? "/" : returnval; //"remove" prefix path
}

typedef struct native_to_js { //for the loop
	duk_ret_t (*native)(duk_context *);
	const char *js;
} native_to_js;

int main(int argc, char *argv[]) {
	{ //Scope this so that when we're done, the buffer is removed, etc
		char buffer[PATH_MAX+1];
		{ //Get initial string for currentpath using realpath as it's how we'll be comparing strings later anyways
			char initialbuff[sizeof(buffer)];
			const char FOLDER_NAME[] = "/computer"; // <- DO NOT PUT A SLASH ON THE END
			getcwd(initialbuff, sizeof(initialbuff));
			strcat(initialbuff, FOLDER_NAME);
			realpath(initialbuff, buffer); //I would just use NULL as the second param, but I was this function to be portable with Windows, this could be a todo, but it's not really worth making...
		}
		
		
		size_t path_size = strlen(buffer);
		CURRENT_PATH = malloc(path_size+1); //+1 for \0
		memcpy(CURRENT_PATH, buffer, path_size);
		
		char *c = CURRENT_PATH;
		for(; *c; c++) {
			if(*c == '\\') *c = '/';
		}
		
		BASE_PATH_SIZE = path_size;
		CURRENT_PATH_SIZE = BASE_PATH_SIZE;
		chdir(CURRENT_PATH); //CHANGE DIR
	}
	
	//Create heap
	duk_context *ctx = duk_create_heap_default();
	
	//Create object
	duk_idx_t os_object = duk_push_object(ctx);
	
	{ //scope this loop, so we can get rid of it after we need it
		const native_to_js all_functions[] = {
			{native_puts, "print"},
			{native_sleep, "sleep"},
			{native_runfile, "run"},
			{native_sandbox, "sandbox"},
			{native_readfile, "read"},
			{native_writefile, "write"},
			{native_removefile, "remove"},
			{native_getline, "getline"},
			{native_getchar, "getchar"},
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
	
	{ //Load plugins
		FILE *f = fopen(ENABLED_PLUGINS_TXT, "r");
		if(f) {
			puts("> Loading Plugins...");
			char path[PATH_MAX] = "plugins/";
			size_t prefixlen = strlen(path);
			char *pathconcat = path + prefixlen;
			char *endofline;
			while(fgets(pathconcat, sizeof(path), f)) {
				endofline = strchr(pathconcat, '\n'); //start at pathconcat because it's the only part that changed
				if(endofline) { //if there's a \n, there could be a \r prior
					*endofline = '\0';
					if(*(--endofline) == '\r') *endofline = '\0';
				}
				
				if(*pathconcat != '#' && *pathconcat != '\0') { //the newlines got removed above
					printf(">> Opening %s\n", pathconcat);
					system_loadlib(ctx, path); //ignore comments
				}
			}
			fclose(f);
			puts("> Done\n");
		} else printf("> Couldn't load %s\n", ENABLED_PLUGINS_TXT);
	}
	
	//Run script
	if(argc > 1) { //interpret and run args
		duk_push_string(ctx, "var shell = {}; shell.resolve = function(a) {return a}; var s = os.run('");
		duk_push_string(ctx, argv[1]);
		duk_push_string(ctx, "'); if(typeof(s) === 'function') s([");
		for(int i = 2; i < argc; i++) { //create "args", like in shell
			duk_push_string(ctx, "'");
			duk_push_string(ctx, argv[i]);
			duk_push_string(ctx, "', ");
		}
		duk_push_string(ctx, "]); ('Complete')"); //return value in brackets
		
		duk_concat(ctx, 4 + ((argc-2) * 3) );
		duk_peval(ctx);
		
	} else duk_peval_string(ctx, "os.run('/startup.js')"); //default
	
	puts(duk_safe_to_string(ctx, -1));
	duk_pop(ctx);

	duk_destroy_heap(ctx);

	return 0;
}