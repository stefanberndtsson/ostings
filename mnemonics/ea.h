#ifndef OSTIS_MNEMONICS_EA_H
#define OSTIS_MNEMONICS_EA_H

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

char *mnemonics_ea(struct cpu *, LONG addr, enum instr_sizes, int, int, int);

#endif /* OSTIS_MNEMONICS_EA_H */
