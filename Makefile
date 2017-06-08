include common.mk

SRC=main.c cpu.c clock.c hw.c common.c
OBJ=$(SRC:.c=.o)

LIBINSTR=instr/libinstr.a

LIB=$(LIBINSTR)
PRG=ostings

all: default

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

default:	$(PRG)

$(PRG):	$(OBJ) $(LIBINSTR)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIB)

$(LIBINSTR):
	make -C instr

clean:
	rm -f $(OBJ) $(PRG) *~
	make -C instr clean

