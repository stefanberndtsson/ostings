#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* LEA */
/* Flags unaffected */

#define OP 0x41C0
#define OP_MASK 0xF1C0

#define BUILD_OP(reg, ea_mode, ea_reg) (0x41c0|(reg<<9)|(ea_mode<<3)|ea_reg)

/*
 * uOPs:
 * EA
 * 
 */

static void add_variant(struct cpu *cpu, int reg, int ea_mode, int ea_reg) {
  struct instr *instr;
  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "LEA");
  ea_addr(instr, ea_mode, ea_reg, INSTR_LONG, REG_AREG(reg));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  cpu_instr_register(cpu, BUILD_OP(reg, ea_mode, ea_reg), 0xFFFF, instr);
}

void instr_lea_setup(struct cpu *cpu) {
  int ea_reg;
  int reg;
  
  for(reg=0;reg<8;reg++) {
    /* Add (An),Ar */
    for(ea_reg=0;ea_reg<8;ea_reg++) {
      add_variant(cpu, reg, EA_MEM, ea_reg);
    }
    /* Add d16(An),Ar */
    for(ea_reg=0;ea_reg<8;ea_reg++) {
      add_variant(cpu, reg, EA_MEM_OFFSET, ea_reg);
    }
    /* Add d8(An,Rn.S),Ar */
    for(ea_reg=0;ea_reg<8;ea_reg++) {
      add_variant(cpu, reg, EA_MEM_OFFSET_REG, ea_reg);
    }
    /* Add $xxxx.W,Ar */
    add_variant(cpu, reg, EA_EXTENDED, EA_SHORT);
    /* Add $xxxx.L,Ar */
    add_variant(cpu, reg, EA_EXTENDED, EA_LONG);
    /* Add d16(PC),Ar */
    add_variant(cpu, reg, EA_EXTENDED, EA_PC_OFFSET);
    /* Add d8(PC,Rn.S),Ar */
    add_variant(cpu, reg, EA_EXTENDED, EA_PC_OFFSET_REG);
  }
}

