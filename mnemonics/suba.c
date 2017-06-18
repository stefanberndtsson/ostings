#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x90c0
#define OP_MASK 0xf0c0

static char *mnemonics_suba(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  int reg;
  char size;

  op = mmu_peek_word(cpu->mmu, addr);
  reg = EA_HIGH_REG(op);
  ea_offset = 0;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, INSTR_LONG, ea_mode, ea_reg, ea_offset);

  size = (op&0x100) ? 'L' : 'W';
  
  mnemonic = ostis_alloc(11+strlen(ea_string));
  snprintf(mnemonic, 11+strlen(ea_string), "SUBA.%c %s,A%d", size, ea_string, reg);

  return mnemonic;
}

void mnemonics_suba_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_suba);
}
