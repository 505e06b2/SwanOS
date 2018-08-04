test.dll: main.c ../../../exec/duktape.dll
	gcc -shared -O2 $^ -I../../../duktape -o $@
	strip $@