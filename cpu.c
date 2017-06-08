#include "common.h"
#include "cpu.h"

void cpu_set_instr_state(struct cpu *cpu, enum instr_states new_state, int counter) {
  if(new_state == INSTR_START &&
     cpu->instr->state != INSTR_END &&
     cpu->instr->state != INSTR_RESET) {
    FATAL("State can only go to START from END or RESET");
  }

  cpu->instr->state = new_state;
  
  if(counter > 0) {
    counter--;
  }
}

void cpu_reset(struct cpu *cpu) {
  cpu->instr->state = INSTR_RESET;
  cpu->instr->counter = 0;
}

struct cpu *cpu_setup() {
  struct cpu *cpu;

  cpu = (struct cpu *)ostis_alloc(sizeof(struct cpu));
  cpu->internal = (struct cpu_internal *)ostis_alloc(sizeof(struct cpu_internal));
  cpu->external = (struct cpu_external *)ostis_alloc(sizeof(struct cpu_external));
  cpu->instr = (struct cpu_instr *)ostis_alloc(sizeof(struct cpu_instr));
  cpu_reset(cpu);
  
  return cpu;
}

void cpu_tick(struct hw *hw) {
  struct cpu *cpu;
  cpu = (struct cpu *)hw->data;
  printf("DEBUG: Ticking away...\n");
  printf("State is %d\n", cpu->instr->state);
}
