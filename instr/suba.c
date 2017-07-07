#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* SUBA */
/* Flags unaffected */

#define OP_REGS 0x90c0
#define OP_MASK_REGS 0xf0f0
#define BUILD_OP(size, reg, ea_mode, ea_reg) (0x90c0|(reg<<9)|(size<<8)|(ea_mode<<3)|ea_reg)

/*
 * uOPs (Regs):
 * NOP
 * Prefetch
 * NOP (subtract)
 * NOP
 * 
 * uOPs (EA):
 * EA-uOPS
 * NOP
 * Prefetch
 * NOP (subtract)
 * NOP (all WORD and immediate-EA for LONG)
 */

void add_ea_variant(struct cpu *cpu, int size_bit, int reg, int ea_mode, int ea_reg) {
  struct instr *instr;
  enum instr_sizes size;
  
  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "SUBA");

  if(size_bit == 0) {
    size = INSTR_WORD;
  } else if(size_bit == 1) {
    size = INSTR_LONG;
  }
  
  ea_read(instr, ea_mode, ea_reg, size, REG_VALUE(0));

  /* DN/AN consumes one uOP in the EA, others do not */
  if(ea_mode != EA_DN && ea_mode != EA_AN) {
    instr_uop_push_nop(instr);
  }
  instr_uop_push_nop(instr); 
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  
  /* At this point value[0] has the value being subtracted. If WORD, sign extend it */
  if(size == INSTR_WORD) {
    instr_uop_push_reg_copy_ext_to_long(instr, REG_WORD_LOW(REG_VALUE(0)), REG_VALUE(0), size);
    instr_uop_push_nop(instr);
  }
  /* Special cases for DN, AN and immediate, that will use an extra two uOPs regardless of size */
  if(size == INSTR_LONG) {
    if(ea_mode == EA_DN || ea_mode == EA_AN || (ea_mode == EA_EXTENDED && ea_reg == EA_IMMEDIATE)) {
      instr_uop_push_nop(instr);
      instr_uop_push_nop(instr);
    }
  }
  //  instr_uop_push_short(instr, subtract, INSTR_UOP_SPECIAL);
  instr_uop_push_sub(instr, REG_VALUE(0), REG_AREG(reg), REG_AREG(reg), size, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, BUILD_OP(size_bit, reg, ea_mode, ea_reg), 0xffff, instr);
}

void instr_suba_setup(struct cpu *cpu) {
  int size,ea_mode,ea_reg,reg;

  for(reg=0;reg<8;reg++) {
    for(ea_mode=0;ea_mode<8;ea_mode++) {
      for(ea_reg=0;ea_reg<8;ea_reg++) {
        for(size=0;size<2;size++) {
          add_ea_variant(cpu, size, reg, ea_mode, ea_reg);
        }
      }
    }
  }
}
