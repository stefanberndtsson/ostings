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

static void set_reset_pin(struct cpu *cpu, LONG data) {
  cpu_set_reset_pin(cpu);
  cpu->exec->uops_pos++;
}

static void clr_reset_pin(struct cpu *cpu, LONG data) {
  cpu_clr_reset_pin(cpu);
  cpu->exec->uops_pos++;
}

struct instr *instr_reset_setup(struct cpu *cpu) {
  int i;
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_unop(instr);
  instr_uop_push_unop(instr);
  instr_uop_push_full(instr, set_reset_pin, INSTR_UOP_SPEC, 0);
  for(i=0;i<RESET_UOPS;i++) {
    instr_uop_push_unop(instr);
  }
  instr_uop_push_full(instr, clr_reset_pin, INSTR_UOP_SPEC, 0);
  instr_uop_push_prog_read(instr);
  instr_uop_push_end(instr);
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

