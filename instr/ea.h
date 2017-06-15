#ifndef OSTIS_INSTR_EA_H
#define OSTIS_INSTR_EA_H

#include "cpu.h"

#define EA_MODE(op) ((op&0x38)>>3)
#define EA_REG(op) (op&0x7)
#define EA_HIGH_REG(op) ((op&0xe00)>>9)
#define EA_HIGH_MODE(op) ((op&0x1c0)>>6)

void ea_read_abs_long(struct instr *, LONG, LONG, enum instr_sizes);
void ea_read_immediate(struct instr *, LONG, enum instr_sizes);

#endif /* OSTIS_INSTR_EA_H */
