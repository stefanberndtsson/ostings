#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP_IM 0x0800
#define OP_IM_MASK 0xffc0
#define OP_REG 0x0100
#define OP_REG_MASK 0xf1c0

static char *mnemonics_btst_i(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  LONG value;

  op = mmu_peek_word(cpu->mmu, addr);
  value = mmu_peek_word(cpu->mmu, addr + 2);
  ea_offset = 2;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, INSTR_BYTE, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(10+strlen(ea_string));
  snprintf(mnemonic, 10+strlen(ea_string), "BTST #%d,%s", value&0xff, ea_string);
  return mnemonic;
}

static char *mnemonics_btst_r(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  int reg;

  op = mmu_peek_word(cpu->mmu, addr);
  reg = EA_HIGH_REG(op);
  ea_offset = 0;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, INSTR_BYTE, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(10+strlen(ea_string));
  snprintf(mnemonic, 10+strlen(ea_string), "BTST D%d,%s", reg, ea_string);
  return mnemonic;
}

void mnemonics_btst_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP_IM, OP_IM_MASK, mnemonics_btst_i);
  mnemonics_register(cpu, OP_REG, OP_REG_MASK, mnemonics_btst_r);
}
