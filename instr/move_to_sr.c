#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* MOVEtoSR */

#define OP 0x46FC
// #define OP_MASK 0xffc0
#define OP_MASK 0xffff

/*
 * uOPs:
 * EA (UNOP, PROG_READ)
 * UNOP (Assemble immediate)
 * UNOP (Set SR)
 * UNOP
 * PROG_READ
 * UNOP
 * PROG_READ
 */

static void set_sr(struct cpu *cpu, LONG data) {
  cpu->internal->sr = cpu->exec->value[0]&0xffff;
  cpu->exec->uops_pos++;
}

struct instr *instr_move_to_sr_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_unop(instr);
  instr_uop_push_prog_read(instr);
  instr_uop_push(instr, INSTR_UOP_IRD_TO_VALUE_LOW, 0);
  instr_uop_push_full(instr, set_sr, INSTR_UOP_SPEC, 0);
  instr_uop_push_unop(instr);
  instr_uop_push_unop(instr);
  instr_uop_push_unop(instr);
  instr_uop_push_prog_read(instr);
  instr_uop_push_end(instr);
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

