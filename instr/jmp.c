#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* JMP */
/* Flags unaffected */

#define OP 0x4EC0
#define OP_MASK 0x4EC0

#define BUILD_OP(ea_mode, ea_reg) (0x4ec0|(ea_mode<<3)|ea_reg)

static void add_variant_mem(struct instr *instr, int ea_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), REG_PC);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
}

static void add_variant_mem_offset(struct instr *instr, int ea_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), REG_PC);
  instr_uop_push_add_word_to_long(instr, REG_IRC_W, REG_PC, REG_PC);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
}

/* TODO: Not implemented */
static void add_variant_mem_offset_reg(struct instr *instr, int ea_reg) {
  unused(instr);
  unused(ea_reg);
}

static void add_variant_pc_offset(struct instr *instr) {
  instr_uop_push_nop(instr);
  instr_uop_push_add_word_to_long(instr, REG_IRC_W, REG_PC, REG_PC);
  instr_uop_push_dec_reg(instr, REG_PC, INSTR_WORD);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
}

/* TODO: Not implemented */
static void add_variant_pc_offset_reg(struct instr *instr) {
  unused(instr);
}

/* TODO: Not implemented */
static void add_variant_short(struct instr *instr) {
  unused(instr);
}

/* TODO: Not implemented */
static void add_variant_long(struct instr *instr) {
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_reg_copy_word(instr, REG_IRD_W, REG_WORD_HIGH(REG_PC));
  instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_LOW(REG_PC));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
}

static void add_variant(struct cpu *cpu, int ea_mode, int ea_reg) {
  struct instr *instr;
  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "JMP");

  switch(ea_mode) {
  case EA_MEM:
    add_variant_mem(instr, ea_reg);
    break;
  case EA_MEM_OFFSET:
    add_variant_mem_offset(instr, ea_reg);
    break;
  case EA_MEM_OFFSET_REG:
    add_variant_mem_offset_reg(instr, ea_reg);
    break;
  case EA_EXTENDED:
    switch(ea_reg) {
    case EA_PC_OFFSET:
      add_variant_pc_offset(instr);
      break;
    case EA_PC_OFFSET_REG:
      add_variant_pc_offset_reg(instr);
      break;
    case EA_SHORT:
      add_variant_short(instr);
      break;
    case EA_LONG:
      add_variant_long(instr);
      break;
    }
    break;
  }
  cpu_instr_register(cpu, BUILD_OP(ea_mode, ea_reg), 0xFFFF, instr);
}

void instr_jmp_setup(struct cpu *cpu) {
  int ea_reg;
  
  /* Add (An) */
  for(ea_reg=0;ea_reg<8;ea_reg++) {
    add_variant(cpu, EA_MEM, ea_reg);
  }
  /* Add d16(An) */
  for(ea_reg=0;ea_reg<8;ea_reg++) {
    add_variant(cpu, EA_MEM_OFFSET, ea_reg);
  }
  /* Add d8(An,Rn.S) */
  for(ea_reg=0;ea_reg<8;ea_reg++) {
    add_variant(cpu, EA_MEM_OFFSET_REG, ea_reg);
  }
  /* Add $xxxx.W */
  add_variant(cpu, EA_EXTENDED, EA_SHORT);
  /* Add $xxxx.L */
  add_variant(cpu, EA_EXTENDED, EA_LONG);
  /* Add d16(PC) */
  add_variant(cpu, EA_EXTENDED, EA_PC_OFFSET);
  /* Add d8(PC,Rn.S) */
  add_variant(cpu, EA_EXTENDED, EA_PC_OFFSET_REG);
}

