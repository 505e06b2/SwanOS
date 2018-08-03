#include <stdio.h>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>

int stdin_char() { //https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar
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