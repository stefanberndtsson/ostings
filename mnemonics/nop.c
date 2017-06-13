#include <stdio.h>
#include "cpu.h"
#include "mnemonics.h"

#define OP 0x4e71
#define OP_MASK 0xffff

static char *mnemonics_nop(struct cpu *cpu, LONG addr) {
  char *mnemonic;

  mnemonic = ostis_alloc(4);
  snprintf(mnemonic, 4, "NOP");
  return mnemonic;
}

void mnemonics_nop_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_nop);
}
