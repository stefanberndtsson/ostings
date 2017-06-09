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
  instr_uop *uops[3] = {
    uop_unop, uop_prog_read, uop_end
  };
  enum instr_uops types[3] = {
    INSTR_UOP_UNOP, INSTR_UOP_PROG_READ, INSTR_UOP_END
  };

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  memcpy(instr->uops, uops, sizeof(uops));
  memcpy(instr->uops_types, types, sizeof(types));

  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

