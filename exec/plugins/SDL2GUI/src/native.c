#include "main.h"

duk_ret_t alt_print(duk_context *ctx) {
	if(duk_get_top(ctx) == 0) return 0;
	printf("ALTPRINT: %s\n", duk_to_string(ctx, 0));
	return 0;
}