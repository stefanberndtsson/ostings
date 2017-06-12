#include <stdio.h>
#include "cpu.h"

static char *mnemonics_boot(struct cpu *cpu) {
  char *mnemonic;

  mnemonic = ostis_alloc(14);
  snprintf(mnemonic, 14, "BOOT-SEQUENCE");
  return mnemonic;
}

void mnemonics_boot_setup(struct cpu *cpu) {
  cpu->exec->mnemonic = mnemonics_boot;
}
