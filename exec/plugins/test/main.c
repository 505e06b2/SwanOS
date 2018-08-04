#include "duktape.h"
#include <stdio.h>

duk_ret_t alt_print(duk_context *ctx) {
	if(duk_get_top(ctx) == 0) return 0;
	printf("ALTPRINT: %s\n", duk_to_string(ctx, 0));
	return 0;
}

void init(duk_context *ctx) {
	duk_bool_t ret = duk_get_global_string(ctx, "os"); //make "os" become -1
	if(ret == 0) return; //no global "os" somehow
	
	duk_push_c_function(ctx, alt_print, DUK_VARARGS);
	ret = duk_put_prop_string(ctx, -2, "print"); //Function is currently -1, obj is -2
	
	duk_pop(ctx); //remove dupe "os", as a reference was brought to the front of the stack
	
	puts(">> Replaced os.print");
}