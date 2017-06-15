#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x41C0
#define OP_MASK 0xF1C0

static char *mnemonics_lea(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  int reg;

  op = mmu_peek_word(cpu->mmu, addr);
  reg = (op&0xe00)>>9;
  ea_offset = 0;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, INSTR_LONG, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(8+strlen(ea_string));
  snprintf(mnemonic, 8+strlen(ea_string), "LEA %s,A%d", ea_string, reg);

  return mnemonic;
}

void mnemonics_lea_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_lea);
}
