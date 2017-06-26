#ifndef OSTIS_INSTR_EA_H
#define OSTIS_INSTR_EA_H

#include "cpu.h"

/* TODO: Duplication with mnemonics */
/* Main */
#define EA_DN 0
#define EA_AN 1
#define EA_MEM 2
#define EA_MEM_INC 3
#define EA_MEM_DEC 4
#define EA_MEM_OFFSET 5
#define EA_MEM_OFFSET_REG 6
#define EA_EXTENDED 7

/* Extended */
#define EA_SHORT 0
#define EA_LONG 1
#define EA_PC_OFFSET 2
#define EA_PC_OFFSET_REG 3
#define EA_IMMEDIATE 4

#define EA_MODE(op) ((op&0x38)>>3)
#define EA_REG(op) (op&0x7)
#define EA_HIGH_REG(op) ((op&0xe00)>>9)
#define EA_HIGH_MODE(op) ((op&0x1c0)>>6)

void ea_read_abs_long(struct instr *, LONG, LONG, enum instr_sizes);
void ea_read_immediate(struct instr *, LONG, enum instr_sizes);
void ea_addr(struct instr *, int, int, enum instr_sizes, LONG);
void ea_read(struct instr *, int, int, enum instr_sizes, LONG);
void ea_write(struct instr *, int, int, enum instr_sizes, LONG);

#endif /* OSTIS_INSTR_EA_H */
