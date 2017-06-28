#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* MOVEQ */

#define OP 0x7000
#define OP_MASK 0xf100
#define BUILD_OP(reg) (0x7000|(reg<<9))

static void add_variant(struct cpu *cpu, int reg) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "MOVEQ");

  instr_uop_push_reg_copy_ext_to_long(instr, REG_IRD_W, REG_DREG(reg), INSTR_BYTE);
  instr_uop_push_set_zn_flags(instr, REG_DREG(reg), INSTR_LONG);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, BUILD_OP(reg), 0xff00, instr);
}

void instr_moveq_setup(struct cpu *cpu) {
  int reg;

  for(reg=0;reg<8;reg++) {
    add_variant(cpu, reg);
  }
}

