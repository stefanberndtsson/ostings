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

static void set_sr(struct cpu *cpu) {
  cpu->internal->sr = cpu->exec->value_w;
  cpu->exec->uops_pos++;
}

struct instr *instr_move_to_sr_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr->uops[0] = uop_unop;
  instr->uops_types[0] = INSTR_UOP_UNOP;
  instr->uops[1] = uop_prog_read; 
  instr->uops_types[1] = INSTR_UOP_PROG_READ;
  instr->uops[2] = uop_assemble_immediate_word;
  instr->uops_types[2] = INSTR_UOP_UNOP;
  instr->uops[3] = set_sr;
  instr->uops_types[3] = INSTR_UOP_UNOP;
  instr->uops[4] = uop_unop;
  instr->uops_types[4] = INSTR_UOP_UNOP;
  instr->uops[5] = uop_unop;
  instr->uops_types[5] = INSTR_UOP_UNOP;
  instr->uops[6] = uop_unop;
  instr->uops_types[6] = INSTR_UOP_UNOP;
  instr->uops[7] = uop_prog_read;
  instr->uops_types[7] = INSTR_UOP_PROG_READ;
  instr->uops[8] = uop_end;
  instr->uops_types[8] = INSTR_UOP_END;

  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

