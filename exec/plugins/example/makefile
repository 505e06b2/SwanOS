OUTNAME = test
CFLAGS = -O2
BINDIR = ofiles

UNAME := $(shell uname)
ifneq ($(UNAME),)
#Unix specific
DEPENDENCIES += $(BINDIR)/duktape.o
CFLAGS += -fPIC
LDFLAGS += -lm
OUTNAME := lib$(OUTNAME).so
else
#Windows
DEPENDENCIES += ../../../exec/duktape.dll
OUTNAME := $(OUTNAME).dll
endif

$(OUTNAME): $(DEPENDENCIES)
	gcc $(CFLAGS) -shared $^ -I../../../duktape $(LDFLAGS) -o $@
	strip $@
	
$(BINDIR)/duktape.o: ../../../duktape/duktape.c
	gcc -c $(CFLAGS) $< -I../../../duktape -o $@