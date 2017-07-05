#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* BTST */
/* Z HIGH if tested bit is 0 */

#define OP_IM 0x0800
#define OP_IM_MASK 0xffc0
#define OP_REG 0x0100
#define OP_REG_MASK 0xf1c0

#define BUILD_IM_OP(ea_mode, ea_reg) (0x0800|(ea_mode<<3)|ea_reg)
#define BUILD_REG_OP(reg, ea_mode, ea_reg) (0x0100|(reg<<9)|(ea_mode<<3)|ea_reg)

static void test_bit(struct uop *uop, struct cpu *cpu) {
  int bitnum;
  LONG value;
  int result;

  result = 0;
  bitnum = cpu->internal->r.value[0]&0x1f;
  if(uop->size == INSTR_BYTE) {
    bitnum = bitnum&0x7;
  }
  value = cpu->internal->r.value[1];
  result = value&(1<<bitnum);
  SET_Z(cpu, CHK_Z(result));
  cpu->exec->uops_pos++;
}

static void add_im_variant(struct cpu *cpu, int ea_mode, int ea_reg) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "BTST #x");

  ea_read_immediate(instr, REG_VALUE(0), INSTR_BYTE);
  ea_read(instr, ea_mode, ea_reg, INSTR_BYTE, REG_VALUE(1));
  instr_uop_push_nop(instr);
  instr_uop_push_full(instr, test_bit, INSTR_UOP_SPECIAL, REG_VALUE(0), REG_VALUE(1), 0, INSTR_BYTE, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, BUILD_IM_OP(ea_mode, ea_reg), 0xffff, instr);
}

static void add_reg_variant(struct cpu *cpu, int reg, int ea_mode, int ea_reg) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "BTST Dn");

  /* When EA is Dn, size is LONG and takes an extra 2 cycles */
  if(ea_mode == EA_DN) {
    instr_uop_push_reg_copy_long(instr, REG_DREG(ea_reg), REG_VALUE(1));
    instr_uop_push_reg_copy_long(instr, REG_DREG(reg), REG_VALUE(0));
    instr_uop_push_full(instr, test_bit, INSTR_UOP_SPECIAL, REG_VALUE(0), REG_VALUE(1), 0, INSTR_LONG, EXT_NONE);
    instr_uop_push_prefetch(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_end(instr);
  } else {
    ea_read(instr, ea_mode, ea_reg, INSTR_BYTE, REG_VALUE(1));
    instr_uop_push_reg_copy_long(instr, REG_DREG(reg), REG_VALUE(0));
    instr_uop_push_full(instr, test_bit, INSTR_UOP_SPECIAL, REG_VALUE(0), REG_VALUE(1), 0, INSTR_BYTE, EXT_NONE);
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch(instr);
    instr_uop_push_end(instr);
  }

  cpu_instr_register(cpu, BUILD_REG_OP(reg, ea_mode, ea_reg), 0xffff, instr);
}

void instr_btst_setup(struct cpu *cpu) {
  int reg,ea_mode,ea_reg;

  for(ea_mode=0;ea_mode<8;ea_mode++) {
    for(ea_reg=0;ea_reg<8;ea_reg++) {
      /* BTST does not work with An */
      if(ea_mode == EA_AN) { continue; }
      add_im_variant(cpu, ea_mode, ea_reg);
    }
  }
  for(reg=0;reg<8;reg++) {
    for(ea_mode=0;ea_mode<8;ea_mode++) {
      for(ea_reg=0;ea_reg<8;ea_reg++) {
        /* BTST does not work with An */
        if(ea_mode == EA_AN) { continue; }
        add_reg_variant(cpu, reg, ea_mode, ea_reg);
      }
    }
  }
}

