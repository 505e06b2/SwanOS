OUTNAME = term
EXECDIR = exec
BINDIR = ofiles
SRCDIR = src
CFLAGS += -O2 -fstrict-aliasing -fomit-frame-pointer
INCLUDEDIR = include
INCLUDES = -I$(INCLUDEDIR) -Iduktape
CC ?= gcc

DEPENDENCIES = $(BINDIR)/main.o $(BINDIR)/native.o

ifeq ($(OS),Windows_NT)
#Windows
DEPENDENCIES += $(BINDIR)/windows.o $(EXECDIR)/duktape.dll
OUTNAME := $(OUTNAME).exe
else
#Unix specific
LDFLAGS += -lm -ldl
DEPENDENCIES += $(BINDIR)/unix.o $(BINDIR)/duktape.o
endif

$(EXECDIR)/$(OUTNAME): $(DEPENDENCIES)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)
	strip $@

$(EXECDIR)/duktape.dll: duktape/duktape.c
	$(CC) $(CFLAGS) -shared $^ $(INCLUDES) -o $@
	strip $@

$(BINDIR)/duktape.o: duktape/duktape.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.c $(INCLUDEDIR)/main.h
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

.PHONY: js

js: 