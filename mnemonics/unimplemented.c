#include <stdio.h>
#include "cpu.h"
#include "mnemonics.h"

#define OP 0x0000
#define OP_MASK 0x0000

static char *mnemonics_unimplemented(struct cpu *cpu, LONG addr) {
  char *mnemonic;

  unused(addr);
  
  mnemonic = ostis_alloc(34);
  snprintf(mnemonic, 34, "Unimplemented instruction: $%04X", cpu->exec->op);
  return mnemonic;
}

void mnemonics_unimplemented_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_unimplemented);
}
