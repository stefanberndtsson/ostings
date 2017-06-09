#include <memory.h>
#include "cpu.h"
#include "uop.h"

/* NOP */

/*
 * uOPs:
 * UNOP
 * PROG_READ
 */

struct instr *instr_boot_setup(WORD op, struct cpu *cpu) {
  struct instr *instr;
  instr_uop *uops[2] = {
    uop_unop, uop_prog_read
  };

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr->op = op;
  memcpy(instr->uops, uops, 12*sizeof(instr_uop *));
  return instr;
}
