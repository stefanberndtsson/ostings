#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* MOVEtoSR */
/* Flags: Set as part of SR */

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

void instr_move_to_sr_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "MOVE xx,SR");

  /* Fetch immediate word */
  ea_read_immediate(instr, REG_VALUE(0), INSTR_WORD);

  /* Write to SR */
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_word(instr, REG_IRD_W, REG_SR_W);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);

  /* Remaining Prefetch */
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  
  cpu_instr_register(cpu, OP, OP_MASK, instr);
}

