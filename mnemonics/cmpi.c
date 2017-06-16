#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x0CB9
#define OP_MASK 0xff00

static char *mnemonics_cmpi(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *ea_string;
  int ea_mode, ea_reg, ea_offset;
  enum instr_sizes size;
  LONG value;

  op = mmu_peek_word(cpu->mmu, addr);
  size = (op&0xc0)>>6;
  value = mmu_peek_word(cpu->mmu, addr + 2);
  if(size == INSTR_LONG) {
    value <<= 16;
    value |= mmu_peek_word(cpu->mmu, addr + 4);
  }
  ea_offset = 4;
  ea_mode = EA_MODE(op);
  ea_reg = EA_REG(op);
  ea_string = mnemonics_ea(cpu, addr, size, ea_mode, ea_reg, ea_offset);
  
  mnemonic = ostis_alloc(19+strlen(ea_string));
  if(size == INSTR_BYTE) {
    snprintf(mnemonic, 19+strlen(ea_string), "CMPI.B #$%02X,%s", value&0xff, ea_string);
  }
  if(size == INSTR_WORD) {
    snprintf(mnemonic, 19+strlen(ea_string), "CMPI.W #$%04X,%s", value&0xffff, ea_string);
  }
  if(size == INSTR_LONG) {
    snprintf(mnemonic, 19+strlen(ea_string), "CMPI.L #$%08X,%s", value, ea_string);
  }
  return mnemonic;
}

void mnemonics_cmpi_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_cmpi);
}
