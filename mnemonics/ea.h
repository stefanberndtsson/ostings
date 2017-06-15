#ifndef OSTIS_MNEMONICS_EA_H
#define OSTIS_MNEMONICS_EA_H

#include "cpu.h"

#define EA_MODE(op) ((op&0x38)>>3)
#define EA_REG(op) (op&0x7)
#define EA_HIGH_REG(op) ((op&0xe00)>>9)
#define EA_HIGH_MODE(op) ((op&0x1c0)>>6)

char *mnemonics_ea(struct cpu *, LONG addr, enum instr_sizes, int, int, int);

#endif /* OSTIS_MNEMONICS_EA_H */
