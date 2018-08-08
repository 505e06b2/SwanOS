OUTNAME = gui
OUTDIR = ..
BINDIR = ofiles
SRCDIR = src
CFLAGS += -O2
LDFLAGS += -lSDL2main -lSDL2
INCLUDEDIR = include
INCLUDES = -I$(INCLUDEDIR) -I../../../../duktape

DEPENDENCIES = $(BINDIR)/main.o $(BINDIR)/native.o $(BINDIR)/eventloop.o

UNAME := $(shell uname)
ifneq ($(UNAME),)
#Unix specific
LDFLAGS += -lm
OUTNAME := lib$(OUTNAME).so
else
#Windows
DEPENDENCIES += ../../../../exec/duktape.dll
LDFLAGS := -Llib -lmingw32 $(LDFLAGS) -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc
OUTNAME := $(OUTNAME).dll
endif

$(OUTDIR)/$(OUTNAME): $(DEPENDENCIES)
	gcc $(CFLAGS) -shared $^ $(INCLUDES) $(LDFLAGS) -o $@
	strip $@

$(BINDIR)/%.o: $(SRCDIR)/%.c $(INCLUDEDIR)/main.h
	gcc $(CFLAGS) -c $< $(INCLUDES) -o $@