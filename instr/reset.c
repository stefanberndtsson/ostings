#include <memory.h>
#include "cpu.h"
#include "instr.h"
#include "uop.h"

/* RESET */

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
}

static void clr_reset_pin(struct cpu *cpu) {
  cpu_clr_reset_pin(cpu);
}

struct instr *instr_reset_setup(WORD op, struct cpu *cpu) {
  int i;
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr->op = op;
  instr->uops[0] = uop_unop;
  instr->uops[1] = uop_unop;
  instr->uops[2] = set_reset_pin;
  for(i=0;i<RESET_UOPS;i++) {
    instr->uops[i+3] = uop_unop;
  }
  instr->uops[RESET_UOPS+3] = clr_reset_pin;
  instr->uops[RESET_UOPS+4] = uop_prog_read;
  return instr;
}

