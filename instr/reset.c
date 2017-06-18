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

static void set_reset_pin(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  cpu_set_reset_pin(cpu);
  cpu->exec->uops_pos++;
}

static void clr_reset_pin(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  cpu_clr_reset_pin(cpu);
  cpu->exec->uops_pos++;
}

void instr_reset_setup(struct cpu *cpu) {
  int i;
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_short(instr, set_reset_pin, INSTR_UOP_SPECIAL);
  for(i=0;i<RESET_UOPS;i++) {
    instr_uop_push_nop(instr);
  }
  instr_uop_push_short(instr, clr_reset_pin, INSTR_UOP_SPECIAL);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
}

