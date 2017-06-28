#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x7000
#define OP_MASK 0xf100

static char *mnemonics_moveq(struct cpu *cpu, LONG addr) {
  WORD op;
  int value,reg;
  char *mnemonic;

  op = mmu_peek_word(cpu->mmu, addr);
  reg = EA_HIGH_REG(op);
  value = SIGN_EXT_WORD(SIGN_EXT_BYTE(op&0xff));
  
  mnemonic = ostis_alloc(16);
  snprintf(mnemonic, 16, "MOVEQ #%d,D%d", value, reg);

  return mnemonic;
}

void mnemonics_moveq_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_moveq);
}
