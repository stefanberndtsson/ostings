#include <stdio.h>
#include "cpu.h"
#include "mnemonics.h"

#define OP 0x4e70
#define OP_MASK 0xffff

static char *mnemonics_reset(struct cpu *cpu, LONG addr) {
  char *mnemonic;

  unused(cpu);
  unused(addr);

  mnemonic = ostis_alloc(6);
  snprintf(mnemonic, 6, "RESET");
  return mnemonic;
}

void mnemonics_reset_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_reset);
}
