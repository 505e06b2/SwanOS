term.exe: duktape.dll bin/main.o bin/native.o
	gcc $^ -o $@ -Iinclude
	strip $@
	
duktape.dll: src/duktape.c
	gcc -shared $^ -Iinclude -o $@
	strip $@

bin/native.o: src/native.c include/main.h
	gcc -c $< -Iinclude -o $@
	
bin/main.o: src/main.c include/main.h
	gcc -c $< -Iinclude -o $@