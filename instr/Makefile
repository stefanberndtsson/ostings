include ../common.mk

CFLAGS := $(CFLAGS) -I..
SRC=unimplemented.c boot.c nop.c reset.c uop.c move_to_sr.c cmpi.c ea.c bcc.c lea.c suba.c jmp.c move_b.c btst.c moveq.c
OBJ=$(SRC:.c=.o)

LIBINSTR=libinstr.a

LIB=$(LIBINSTR)

all:	$(LIBINSTR)

$(LIBINSTR):	$(OBJ)
	$(AR) cr $@ $(OBJ)

clean:
	rm -f $(OBJ) $(LIBINSTR) *~
