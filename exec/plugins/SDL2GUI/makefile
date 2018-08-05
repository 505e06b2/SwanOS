gui.dll: main.c ../../../exec/duktape.dll
	gcc -shared $^ -I../../../duktape -ISDL2/include -LSDL2 -lmingw32 -lSDL2main -lSDL2 -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc -o $@
	strip $@
