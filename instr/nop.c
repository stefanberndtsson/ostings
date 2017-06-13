#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"

/* NOP */

#define OP 0x4e71
#define OP_MASK 0xffff

/*
 * uOPs:
 * UNOP
 * PROG_READ
 */

struct instr *instr_nop_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_unop(instr);
  instr_uop_push_prog_read(instr);

  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

