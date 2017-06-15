#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* CMPI */

#define OP 0x0C00
#define OP_MASK 0xff00

/*
 * uOPs (LONG):
 * UNOP (Assemble left value)
 * PROG_READ
 * UNOP (Assemble left value)
 * PROG_READ
 * UNOP (Assemble address)
 * PROG_READ (EA)
 * UNOP (Assemble address)
 * PROG_READ (EA)
 * UNOP
 * READ_WORD
 * UNOP (Assemble right value)
 * READ_WORD
 * UNOP (Assemble right value)
 * PROG_READ
 */

/* TODO: Implement comparison */
static void compare(struct uop *uop, struct cpu *cpu) {
  cpu->exec->uops_pos++;
}

struct instr *instr_cmpi_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;

  /* Fetch LONG immediate value */
  ea_read_immediate(instr, REG_VALUE_H_TO_REG_W(0), INSTR_LONG);

  /* Fetch EA */
  ea_read_abs_long(instr, 1, REG_VALUE_H_TO_REG_W(2), INSTR_LONG);

  /* Do comparison between VALUE[0] and VALUE[2] */
  instr_uop_push_full(instr, compare, INSTR_UOP_SPECIAL, REG_VALUE_TO_REG_L(1), REG_VALUE_TO_REG_L(2), INSTR_LONG, EXT_NONE);

  /* End with prefetch */
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

