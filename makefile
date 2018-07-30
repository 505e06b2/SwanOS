OUTNAME = term
EXECDIR = exec
BINDIR = ofiles
SRCDIR = src
CFLAGS += -O2 -fstrict-aliasing -fomit-frame-pointer
INCLUDEDIR = include
INCLUDES = -I$(INCLUDEDIR) -Iduktape

DEPENDENCIES = $(BINDIR)/main.o $(BINDIR)/native.o

UNAME := $(shell uname)
ifneq ($(UNAME),)
#Unix specific
LDFLAGS += -lm
DEPENDENCIES += $(BINDIR)/duktape.o
else
#Windows
DEPENDENCIES += $(BINDIR)/windows.o $(EXECDIR)/duktape.dll
OUTNAME := $(OUTNAME).exe
endif

$(EXECDIR)/$(OUTNAME): $(DEPENDENCIES)
	gcc $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)
	strip $@

$(EXECDIR)/duktape.dll: duktape/duktape.c
	gcc $(CFLAGS) -shared $^ $(INCLUDES) -o $@
	strip $@

$(BINDIR)/duktape.o: duktape/duktape.c
	gcc $(CFLAGS) -c $< $(INCLUDES) -o $@
	
$(BINDIR)/%.o: $(SRCDIR)/%.c $(INCLUDEDIR)/main.h
	gcc $(CFLAGS) -c $< $(INCLUDES) -o $@
