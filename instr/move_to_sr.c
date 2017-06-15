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
 * EA (NOP, PROG_READ)
 * NOP (Assemble immediate)
 * NOP (Set SR)
 * NOP
 * PREFETCH
 * NOP
 * PREFETCH
 */

struct instr *instr_move_to_sr_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;

  /* Fetch immediate word */
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);

  /* Write to SR */
  instr_uop_push_nop(instr);
  instr_uop_push(instr, INSTR_UOP_REG_COPY, REG_IRD_TO_REG_W, REG_SR_TO_REG_W, INSTR_WORD, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);

  /* Remaining Prefetch */
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

