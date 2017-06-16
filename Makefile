include common.mk

SRC=main.c cpu.c clock.c hw.c common.c mmu.c ram.c rom.c bootrom.c mnemonics.c cart.c instr.c
OBJ=$(SRC:.c=.o)

LIBINSTR=instr/libinstr.a
LIBMNEMONICS=mnemonics/libmnemonics.a

LIB=$(LIBINSTR) $(LIBMNEMONICS)
PRG=ostings

.SUFFIXES:

all: clean default

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

default:	$(PRG)

$(PRG):	$(OBJ) $(LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIB)

$(LIBINSTR):
	make -C instr

$(LIBMNEMONICS):
	make -C mnemonics

clean::
	rm -f $(OBJ) $(PRG) *~ core
	make -C instr clean
	make -C mnemonics clean

include test.mk
