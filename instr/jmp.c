#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* LEA */

#define OP 0x4EC0
#define OP_MASK 0x4EC0

#define BUILD_OP(ea_mode, ea_reg) (0x4ec0|(ea_mode<<3)|ea_reg)

/*
 * uOPs:
 * NOP (not for $xxxx.L)
 * NOP (only for d8(An,Rn.S) and d8(PC,Rn.S))
 * EA
 * set_pc
 * Prefetch
 */

/*  n, p, n, p, rc, p
 */

static void add_variant(struct cpu *cpu, int ea_mode, int ea_reg) {
  struct instr *instr;
  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  if(!(ea_mode == EA_EXTENDED && ea_reg == EA_LONG)) {
    instr_uop_push_nop(instr);
  }
  if(ea_mode == EA_MEM_OFFSET_REG || (ea_mode == EA_EXTENDED && ea_reg == EA_PC_OFFSET_REG)) {
    instr_uop_push_nop(instr);
  }
  ea_addr_jmp(instr, ea_mode, ea_reg, INSTR_LONG, REG_VALUE(0));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  cpu_instr_register(cpu, BUILD_OP(ea_mode, ea_reg), 0xFFFF, instr);
}

void instr_jmp_setup(struct cpu *cpu) {
  int ea_reg;
  
  /* Add (An),Ar */
  for(ea_reg=0;ea_reg<8;ea_reg++) {
    add_variant(cpu, EA_MEM, ea_reg);
  }
  /* Add d16(An),Ar */
  for(ea_reg=0;ea_reg<8;ea_reg++) {
    add_variant(cpu, EA_MEM_OFFSET, ea_reg);
  }
  /* Add d8(An,Rn.S),Ar */
  for(ea_reg=0;ea_reg<8;ea_reg++) {
    add_variant(cpu, EA_MEM_OFFSET_REG, ea_reg);
  }
  /* Add $xxxx.W,Ar */
  add_variant(cpu, EA_EXTENDED, EA_SHORT);
  /* Add $xxxx.L,Ar */
  add_variant(cpu, EA_EXTENDED, EA_LONG);
  /* Add d16(PC),Ar */
  add_variant(cpu, EA_EXTENDED, EA_PC_OFFSET);
  /* Add d8(PC,Rn.S),Ar */
  add_variant(cpu, EA_EXTENDED, EA_PC_OFFSET_REG);
}

