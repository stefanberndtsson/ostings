#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x4EC0
#define OP_MASK 0xFFC0

static char *mnemonics_jmp(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;

  op = mmu_peek_word(cpu->mmu, addr);
  ea_offset = 0;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, INSTR_LONG, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(5+strlen(ea_string));
  snprintf(mnemonic, 5+strlen(ea_string), "JMP %s", ea_string);

  return mnemonic;
}

void mnemonics_jmp_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_jmp);
}
