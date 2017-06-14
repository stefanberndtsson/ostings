#include "instr.h"

/* TODO: Duplication with Mnemonics */

#define DREG_TO_IDX(reg) (reg)
#define AREG_TO_IDX(reg) (reg+8)
#define IDX_TO_DREG(reg) (reg)
#define IDX_TO_AREG(reg) (reg-8)

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

/* Specialised uOPs for use in EA */
static void ea_uop_areg_to_value0_postinc_word(struct cpu *cpu, LONG reg_num) {
  cpu->exec->value[0] = cpu->internal->r.a[reg_num];
  cpu->internal->r.a[reg_num] += 2;
  cpu->exec->uops_pos++;
}

static void ea_uop_areg_to_value0_postinc_long(struct cpu *cpu, LONG reg_num) {
  cpu->exec->value[0] = cpu->internal->r.a[reg_num];
  cpu->internal->r.a[reg_num] += 4;
  cpu->exec->uops_pos++;
}

static void ea_uop_areg_dec_word(struct cpu *cpu, LONG reg_num) {
  cpu->internal->r.a[reg_num] -= 2;
  cpu->exec->uops_pos++;
}

static void ea_uop_areg_dec_long(struct cpu *cpu, LONG reg_num) {
  cpu->internal->r.a[reg_num] -= 4;
  cpu->exec->uops_pos++;
}

/* EA Read
 *
 * Address will be written to VALUE[0]
 * If BYTE/WORD, result will be in external->data
 * If LONG, (VALUE[1]&0xFFFF0000)|external->data will combine into result
 * 
 */

void ea_read_uop_mem_word(struct instr *instr, int ea_reg) {
  instr_uop_push(instr, INSTR_UOP_AREG_TO_VALUE0_LONG, ea_reg);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
}

void ea_read_uop_mem_long(struct instr *instr, int ea_reg) {
  instr_uop_push(instr, INSTR_UOP_AREG_TO_VALUE0_LONG, ea_reg);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_HIGH, 1);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, 0);
}

void ea_read_uop_mem_inc_word(struct instr *instr, int ea_reg) {
  instr_uop_push_full(instr, ea_uop_areg_to_value0_postinc_word, INSTR_UOP_SPEC_EA, ea_reg);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
}

void ea_read_uop_mem_inc_long(struct instr *instr, int ea_reg) {
  instr_uop_push_full(instr, ea_uop_areg_to_value0_postinc_long, INSTR_UOP_SPEC_EA, AREG_TO_IDX(ea_reg));
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_HIGH, 1);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, 0);
}

void ea_read_uop_mem_dec_word(struct instr *instr, int ea_reg) {
  instr_uop_push_full(instr, ea_uop_areg_dec_word, INSTR_UOP_SPEC_EA, ea_reg);
  instr_uop_push(instr, INSTR_UOP_AREG_TO_VALUE0_LONG, ea_reg);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
}

void ea_read_uop_mem_dec_long(struct instr *instr, int ea_reg) {
  instr_uop_push_full(instr, ea_uop_areg_dec_long, INSTR_UOP_SPEC_EA, ea_reg);
  instr_uop_push(instr, INSTR_UOP_AREG_TO_VALUE0_LONG, ea_reg);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_HIGH, 1);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, 0);
}

#if 0
/* Dn will never cause an extra uOP */
static int ea_dn(int ea_reg) {
  return 0;
}

/* An will never cause an extra uOP */
static int ea_an(int ea_reg) {
  return 0;
}

/* (An) will add two or four uOPs, UNOP + READ_WORD, x2 for LONG */
static int ea_mem(int ea_reg, enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 4;
  } else {
    return 2;
  }
}

/* (An)+ will add two or four uOPs, UNOP + READ_WORD, x2 for LONG */
static int ea_mem_inc(int ea_reg, enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 4;
  } else {
    return 2;
  }
}

/* -(An) will add three or five uOPs, one UNOP then (UNOP + READ_WORD, x2 for LONG) */
static int ea_mem_dec(int ea_reg, enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 5;
  } else {
    return 3;
  }
}

/* d16(An) will add four or six uOPs, UNOP + PROG_READ, then (UNOP + READ_WORD, x2 for LONG) */
static int ea_mem_offset(int ea_reg, enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 6;
  } else {
    return 4;
  }
}

/* d8(An,Xn.S) will add five or seven uOPs, UNOP + UNOP + PROG_READ, then (UNOP + READ_WORD, x2 for LONG) */
static int ea_mem_offset_reg(enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 7;
  } else {
    return 5;
  }
}

/* $xxxx.W will add four or six uOPs, UNOP + PROG_READ, then (UNOP + READ_WORD, x2 for LONG) */
static int ea_short(enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 6;
  } else {
    return 4;
  }
}

/* $xxxx.L will add six or eight uOPs, (UNOP + PROG_READ, x2), then (UNOP + READ_WORD, x2 for LONG) */
static int ea_long(enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 8;
  } else {
    return 6;
  }
}

/* d16(PC) same as d16(An) */
static int ea_pc_offset(enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 6;
  } else {
    return 4;
  }
}

/* d8(PC,Xn.S) same as d8(An,Xn.S) */
static int ea_pc_offset_reg(enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 7;
  } else {
    return 5;
  }
}

/* #xxxx will add two or four uOPs, UNOP + PROG_READ, x2 for LONG */
static int ea_immediate(enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  if(size == INSTR_LONG) {
    return 4;
  } else {
    return 2;
  }
}

/*
 * Add the relevant uOPs for the current EA and types to the queue for the instruction,
 * then return how many positions was added.
 */
int ea_uops(int ea_mode, int ea_reg, enum instr_sizes size, int uops_count, instr_uop **uops, enum instr_uops *types) {
  switch(ea_mode) {
  case EA_DN:
    return ea_dn(ea_reg);
  case EA_AN:
    return ea_an(ea_reg);
  case EA_MEM:
    return ea_mem(ea_reg, size, uops_count, uops, types);
  case EA_MEM_INC:
    return ea_mem_inc(ea_reg, size, uops_count, uops, types);
  case EA_MEM_DEC:
    return ea_mem_dec(ea_reg, size, uops_count, uops, types);
  case EA_MEM_OFFSET:
    return ea_mem_offset(cpu, ea_reg, size, uops_count, uops, types);
  case EA_MEM_OFFSET_REG:
    return ea_mem_offset_reg(cpu, ea_reg, size, uops_count, uops, types);
  case EA_EXTENDED:
    switch(ea_reg) {
    case EA_SHORT:
      return ea_short(cpu, size, uops_count, uops, types);
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
  return 0;
}

int ea_uops_instr(int ea_mode, int ea_reg, struct instr *instr) {
  int uops_count;
  int i;

  for(i=0,uops_count=0;instr->uops_types[i] != INSTR_UOP_END;i++,uops_count++);
  return ea_uops(ea_mode, ea_reg, instr->size, uops_count, instr->uops, instr->uops_types);
}
#endif
