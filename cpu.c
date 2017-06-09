#include "common.h"
#include "cpu.h"
#include "instr.h"

void cpu_set_reset_pin(struct cpu *cpu) {
  /* TODO: All */
  return;
}

void cpu_clr_reset_pin(struct cpu *cpu) {
  /* TODO: All */
  return;
}

struct cpu *cpu_setup(struct hw **hws) {
  struct cpu *cpu;

  cpu = (struct cpu *)ostis_alloc(sizeof(struct cpu));
  cpu->internal = (struct cpu_internal *)ostis_alloc(sizeof(struct cpu_internal));
  cpu->internal->main_state = CPU_RESET;
  
  cpu->external = (struct cpu_external *)ostis_alloc(sizeof(struct cpu_external));
  cpu->exec = (struct cpu_exec *)ostis_alloc(sizeof(struct cpu_exec));
  cpu->exec->instr = instr_boot_setup(0, cpu);

  cpu->hws = hws;
  
  return cpu;
}

void cpu_tick(struct hw *hw) {
  //  struct cpu *cpu;
  //  cpu = (struct cpu *)hw->data;
  printf("DEBUG: Ticking away...\n");
}
