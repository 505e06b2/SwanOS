#include "main.h" //since most functions needed here will already have the includes pulled
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <dlfcn.h> //Load libs

int system_getchar() { //https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar
	int c;   
    static struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt); //save current term params
    newt = oldt; //clone settings so we can restore
	
    newt.c_lflag &= ~(ICANON | ECHO); //ICANON: don't buffer stdin; ECHO: don't echo output
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); //set new params
	
	c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); //restore

    return c;
}

void system_sleep(unsigned int milliseconds) {
	useconds_t useconds = milliseconds * 1000;
	usleep(useconds);
}

void system_loadlib(duk_context *ctx, const char *path) {
	void *lib = dlopen(path, RTLD_LAZY);
	if(lib == NULL)  return;
	void *findfunc = dlsym(lib, "init");
	if(findfunc == NULL)  return;
	(void (*)(duk_context *))findfunc(ctx);
}
