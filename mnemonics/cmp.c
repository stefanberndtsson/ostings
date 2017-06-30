#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0xB000
#define OP_MASK 0xf100

static char *mnemonics_cmp(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  int reg;
  enum instr_sizes size;
  char size_char;

  op = mmu_peek_word(cpu->mmu, addr);
  reg = EA_HIGH_REG(op);
  size = (op&0xc0)>>6;

  if(size == INSTR_BYTE) { size_char = 'B'; }
  if(size == INSTR_WORD) { size_char = 'W'; }
  if(size == INSTR_LONG) { size_char = 'L'; }
  
  ea_offset = 0;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, size, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(19+strlen(ea_string));
  snprintf(mnemonic, 19+strlen(ea_string), "CMP.%c %s,D%d", size_char, ea_string, reg);

  return mnemonic;
}

void mnemonics_cmp_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_cmp);
}
