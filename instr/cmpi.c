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

static void compare(struct cpu *cpu) {
  cpu->exec->uops_pos++;
}

struct instr *instr_cmpi_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_unop(instr);
  instr_uop_push_prog_read(instr);
  instr_uop_push(instr, INSTR_UOP_IRD_TO_VALUE_HIGH, 0);
  instr_uop_push_prog_read(instr);
  instr_uop_push(instr, INSTR_UOP_IRD_TO_VALUE_LOW, 0);
  instr_uop_push_prog_read(instr);
  instr_uop_push(instr, INSTR_UOP_IRD_TO_VALUE_HIGH, 1);
  instr_uop_push_prog_read(instr);
  instr_uop_push(instr, INSTR_UOP_IRD_TO_VALUE_LOW, 1);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 1);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_HIGH, 2);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, 1);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_LOW, 2);
  instr_uop_push_prog_read(instr);
  instr_uop_push_end(instr);
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

