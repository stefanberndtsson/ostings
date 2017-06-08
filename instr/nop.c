#include "cpu.h"

/* NOP */

/*
 * States:
 * INSTR_START
 * INSTR_UNOP
 * INSTR_PROG_READ
 * INSTR_END
 */

void execute(struct cpu *cpu, enum instr_states state) {
  if(state == INSTR_START) {
    cpu_set_instr_state(cpu, INSTR_UNOP, 0);
    return;
  }
}
