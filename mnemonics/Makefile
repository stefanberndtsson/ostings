include ../common.mk

CFLAGS := $(CFLAGS) -I..
SRC=unimplemented.c ea.c boot.c nop.c reset.c move_to_sr.c cmpi.c bcc.c lea.c suba.c jmp.c move_b.c
OBJ=$(SRC:.c=.o)

LIBMNEMONICS=libmnemonics.a

LIB=$(LIBMNEMONICS)

all:	$(LIBMNEMONICS)

$(LIBMNEMONICS):	$(OBJ)
	$(AR) cr $@ $(OBJ)

clean:
	rm -f $(OBJ) $(LIBMNEMONICS) *~
