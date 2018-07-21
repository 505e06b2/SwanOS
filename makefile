term.exe: duktape.dll bin/main.o 
	gcc $^ -o $@ -Iinclude
	strip $@
	
duktape.dll: src/duktape.c
	gcc -shared $^ -Iinclude -o $@
	strip $@
	
bin/main.o: src/main.c include/main.h
	gcc -c $< -Iinclude -o $@