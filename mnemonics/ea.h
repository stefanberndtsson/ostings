#ifndef OSTIS_EA_H
#define OSTIS_EA_H

#include "cpu.h"

#define EA_MODE(op) ((op&0x38)>>3)
#define EA_REG(op) (op&0x7)

char *mnemonics_ea(struct cpu *, enum instr_sizes, int, int);

#endif /* OSTIS_EA_H */
