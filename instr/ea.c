#include "instr.h"

/* TODO: Duplication with Mnemonics */
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
