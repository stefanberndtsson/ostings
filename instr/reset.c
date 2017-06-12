#include <memory.h>
#include "cpu.h"
#include "instr.h"
#include "uop.h"

/* RESET */

#define OP 0x4e70
#define OP_MASK 0xffff

/*
 * uOPs:
 * UNOP
 * UNOP
 * UNOP (Set reset pin)
 * UNOPx61
 * UNOP (Clr reset pin)
 * PROG_READ
 */

/* Cycles to wait during reset execution */
#define RESET_DELAY 124
#define RESET_UOPS ((RESET_DELAY/2)-1)

static void set_reset_pin(struct cpu *cpu) {
  cpu_set_reset_pin(cpu);
  cpu->exec->uops_pos++;
}

static void clr_reset_pin(struct cpu *cpu) {
  cpu_clr_reset_pin(cpu);
  cpu->exec->uops_pos++;
}

struct instr *instr_reset_setup(struct cpu *cpu) {
  int i;
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr->uops[0] = uop_unop;
  instr->uops_types[0] = INSTR_UOP_UNOP;
  instr->uops[1] = uop_unop;
  instr->uops_types[1] = INSTR_UOP_UNOP;
  instr->uops[2] = set_reset_pin;
  instr->uops_types[2] = INSTR_UOP_UNOP;
  for(i=0;i<RESET_UOPS;i++) {
    instr->uops[i+3] = uop_unop;
    instr->uops_types[i+3] = INSTR_UOP_UNOP;
  }
  instr->uops[RESET_UOPS+3] = clr_reset_pin;
  instr->uops_types[RESET_UOPS+3] = INSTR_UOP_UNOP;
  instr->uops[RESET_UOPS+4] = uop_prog_read;
  instr->uops_types[RESET_UOPS+4] = INSTR_UOP_PROG_READ;
  instr->uops[RESET_UOPS+5] = uop_end;
  instr->uops_types[RESET_UOPS+5] = INSTR_UOP_END;
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

