#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "ea.h"

#define OP 0x46FC
#define OP_MASK 0xffc0

static char *mnemonics_move_to_sr(struct cpu *cpu) {
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg;

  ea_mode = EA_MODE(cpu->exec->op);
  ea_reg = EA_REG(cpu->exec->op);
  ea_string = mnemonics_ea(cpu, INSTR_WORD, ea_mode, ea_reg);
  
  mnemonic = ostis_alloc(9+strlen(ea_string));
  snprintf(mnemonic, 9+strlen(ea_string), "MOVE %s,SR", ea_string);
  return mnemonic;
}

void mnemonics_move_to_sr_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_move_to_sr);
}
