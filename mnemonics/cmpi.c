#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x0CB9
#define OP_MASK 0xff00

static char *mnemonics_cmpi(struct cpu *cpu) {
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  enum instr_sizes size;
  LONG value;

  size = (cpu->exec->op&0xc0)>>6;

  value = mmu_peek_word(cpu->mmu, cpu->exec->instr_addr + 2);
  if(size == INSTR_LONG) {
    value <<= 16;
    value |= mmu_peek_word(cpu->mmu, cpu->exec->instr_addr + 4);
  }
  ea_offset = 4;
  ea_mode = EA_MODE(cpu->exec->op);
  ea_reg = EA_REG(cpu->exec->op);
  ea_string = mnemonics_ea(cpu, size, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(17+strlen(ea_string));
  if(size == INSTR_BYTE) {
    snprintf(mnemonic, 17+strlen(ea_string), "CMPI #$%02X,%s", value&0xff, ea_string);
  }
  if(size == INSTR_WORD) {
    snprintf(mnemonic, 17+strlen(ea_string), "CMPI #$%04X,%s", value&0xffff, ea_string);
  }
  if(size == INSTR_LONG) {
    snprintf(mnemonic, 17+strlen(ea_string), "CMPI #$%08X,%s", value, ea_string);
  }
  return mnemonic;
}

void mnemonics_cmpi_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_cmpi);
}
