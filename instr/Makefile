include ../common.mk

CFLAGS := $(CFLAGS) -I..
SRC=nop.c
OBJ=$(SRC:.c=.o)

LIBINSTR=libinstr.a

LIB=$(LIBINSTR)

all:	$(LIBINSTR)

$(LIBINSTR):	$(OBJ)
	$(AR) cr $@ $(OBJ)

clean:
	rm -f $(OBJ) $(LIBINSTR) *~
