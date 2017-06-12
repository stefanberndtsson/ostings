#include <stdio.h>
#include "cpu.h"
#include "mmu.h"
#include "instr.h"

/* TODO: Duplication with instruction */
/* Main */
#define EA_DN 0
#define EA_AN 1
#define EA_MEM 2
#define EA_MEM_INC 3
#define EA_MEM_DEC 4
#define EA_MEM_OFFSET 5
#define EA_MEM_OFFSET_REG 6
#define EA_EXTENDED 7

/* Extended */
#define EA_SHORT 0
#define EA_LONG 1
#define EA_PC_OFFSET 2
#define EA_PC_OFFSET_REG 3
#define EA_IMMEDIATE 4

static int32_t sign_extend_byte(BYTE value) {
  int32_t out;

  out = value;
  
  if(out&0x80) {
    out = out | 0xffffff00;
  }

  return out;
}

static int32_t sign_extend_word(WORD value) {
  int32_t out;

  out = value;
  
  if(out&0x8000) {
    out = out | 0xffff0000;
  }

  return out;
}

static char *ea_dn(int ea_reg) {
  char *ea;

  ea = (char *)ostis_alloc(3);
  snprintf(ea, 3, "D%d", ea_reg);
  return ea;
}

static char *ea_an(int ea_reg) {
  char *ea;

  ea = (char *)ostis_alloc(3);
  snprintf(ea, 3, "A%d", ea_reg);
  return ea;
}

static char *ea_mem(int ea_reg) {
  char *ea;

  ea = (char *)ostis_alloc(5);
  snprintf(ea, 5, "(A%d)", ea_reg);
  return ea;
}

static char *ea_mem_inc(int ea_reg) {
  char *ea;

  ea = (char *)ostis_alloc(6);
  snprintf(ea, 6, "(A%d)+", ea_reg);
  return ea;
}

static char *ea_mem_dec(int ea_reg) {
  char *ea;

  ea = (char *)ostis_alloc(6);
  snprintf(ea, 6, "-(A%d)", ea_reg);
  return ea;
}

static char *ea_mem_offset(struct cpu *cpu, int ea_reg) {
  char *ea;
  int32_t offset;

  offset = sign_extend_word(mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2));
  
  ea = (char *)ostis_alloc(11);
  snprintf(ea, 11, "%d(A%d)", offset, ea_reg);
  return ea;
}

static char *ea_mem_offset_reg(struct cpu *cpu, int ea_reg) {
  char *ea;
  WORD extension;
  int offset;
  int offset_reg;
  char offset_reg_type;
  char offset_reg_size;

  extension = mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2);
  offset = sign_extend_byte(extension & 0xff);
  offset_reg = (extension & 0x7000)>>12;
  offset_reg_type = extension&0x8000 ? 'A' : 'D';
  offset_reg_size = extension&0x800 ? 'L' : 'W';

  ea = (char *)ostis_alloc(14);
  snprintf(ea, 14, "%d(A%d,%c%d.%c)", offset, ea_reg, offset_reg_type, offset_reg, offset_reg_size);
  return ea;
}

static char *ea_short(struct cpu *cpu) {
  char *ea;
  WORD offset;

  offset = mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2);
  
  ea = (char *)ostis_alloc(8);
  snprintf(ea, 8, "$%04X.W", offset);
  return ea;
}

static char *ea_long(struct cpu *cpu) {
  char *ea;
  LONG offset;

  offset = mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2);
  offset <<= 16;
  offset |= mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+4);
  
  ea = (char *)ostis_alloc(12);
  snprintf(ea, 12, "$%08X.L", offset);
  return ea;
}

static char *ea_pc_offset(struct cpu *cpu) {
  char *ea;
  int32_t offset;

  offset = sign_extend_word(mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2));
  
  ea = (char *)ostis_alloc(11);
  snprintf(ea, 11, "%d(PC)", offset);
  return ea;
}

static char *ea_pc_offset_reg(struct cpu *cpu) {
  char *ea;
  WORD extension;
  int offset;
  int offset_reg;
  char offset_reg_type;
  char offset_reg_size;

  extension = mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2);
  offset = sign_extend_byte(extension & 0xff);
  offset_reg = (extension & 0x7000)>>12;
  offset_reg_type = extension&0x8000 ? 'A' : 'D';
  offset_reg_size = extension&0x800 ? 'L' : 'W';

  ea = (char *)ostis_alloc(14);
  snprintf(ea, 14, "%d(PC,%c%d.%c)", offset, offset_reg_type, offset_reg, offset_reg_size);
  return ea;
}

static char *ea_immediate(struct cpu *cpu, enum instr_sizes size) {
  char *ea;
  LONG value;

  value = mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+2);
  if(size == INSTR_LONG) {
    value <<= 16;
    value |= mmu_peek_word(cpu->mmu, cpu->exec->instr_addr+4);
  }
  
  ea = (char *)ostis_alloc(8);
  if(size == INSTR_WORD) {
    snprintf(ea, 7, "#$%04X", value&0xffff);
  } else {
    snprintf(ea, 7, "#$%08X", value);
  }
  return ea;
}

char *mnemonics_ea(struct cpu *cpu, enum instr_sizes size, int ea_mode, int ea_reg) {
  switch(ea_mode) {
  case EA_DN:
    return ea_dn(ea_reg);
  case EA_AN:
    return ea_an(ea_reg);
  case EA_MEM:
    return ea_mem(ea_reg);
  case EA_MEM_INC:
    return ea_mem_inc(ea_reg);
  case EA_MEM_DEC:
    return ea_mem_dec(ea_reg);
  case EA_MEM_OFFSET:
    return ea_mem_offset(cpu, ea_reg);
  case EA_MEM_OFFSET_REG:
    return ea_mem_offset_reg(cpu, ea_reg);
  case EA_EXTENDED:
    switch(ea_reg) {
    case EA_SHORT:
      return ea_short(cpu);
    case EA_LONG:
      return ea_long(cpu);
    case EA_PC_OFFSET:
      return ea_pc_offset(cpu);
    case EA_PC_OFFSET_REG:
      return ea_pc_offset_reg(cpu);
    case EA_IMMEDIATE:
      return ea_immediate(cpu, size);
    }
  }
  /* TODO: Better error handling! */
  return "<UNKNOWN-EA>";
}
