#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* MOVE.B */
/* Flags:
 * X - Not affected
 * N - If value is negative
 * Z - If value is 0
 * V - Cleared
 * C - Cleared
 */


#define OP 0x1000
#define OP_MASK 0xf000
#define BUILD_OP(src_ea_mode, src_ea_reg, dst_ea_mode, dst_ea_reg) (0x1000|(dst_ea_reg<<9)|(dst_ea_mode<<6)|(src_ea_mode<<3)|src_ea_reg)

static void add_ea_variant(struct cpu *cpu, int src_ea_mode, int src_ea_reg, int dst_ea_mode, int dst_ea_reg) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "MOVE.B");

  ea_move(instr, src_ea_mode, src_ea_reg, dst_ea_mode, dst_ea_reg, INSTR_BYTE);
  /* The first uNOP is replaced with a reg_copy in ea_move when dst is Dn */
  if(dst_ea_mode != EA_DN) {
    instr_uop_push_nop(instr);
  }
  /* At this point, value having been moved is in r.value[0] */
  instr_uop_push_set_basic_flags(instr, REG_VALUE(0), INSTR_BYTE);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, BUILD_OP(src_ea_mode, src_ea_reg, dst_ea_mode, dst_ea_reg), 0xffff, instr);
}

void instr_move_b_setup(struct cpu *cpu) {
  int src_ea_mode,src_ea_reg;
  int dst_ea_mode,dst_ea_reg;

  for(src_ea_mode=0;src_ea_mode<8;src_ea_mode++) {
    for(src_ea_reg=0;src_ea_reg<8;src_ea_reg++) {
      for(dst_ea_mode=0;dst_ea_mode<8;dst_ea_mode++) {
        for(dst_ea_reg=0;dst_ea_reg<8;dst_ea_reg++) {
          add_ea_variant(cpu, src_ea_mode, src_ea_reg, dst_ea_mode, dst_ea_reg);
        }
      }
    }
  }
}

