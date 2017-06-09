#include "common.h"
#include "cpu.h"
#include "instr.h"

static char *uops_names[INSTR_UOP_MAX_COUNT] = {
  "END",
  "UNOP",
  "UNOP-CNT",
  "PROGRAM-READ",
  "READ_LOW_BYTE",
  "READ_HIGH_BYTE",
  "READ_WORD",
  "WRITE_LOW_BYTE",
  "WRITE_HIGH_BYTE",
  "WRITE_WORD",
  "EXECUTE",
  "BOOT_PREFETCH"
};

static char *states[3] = {
  "RESET",
  "HALT",
  "RUNNING"
};

void cpu_debug_info(struct cpu *cpu) {
  int i;
  printf("-----------------------------------------------------------------------\n");
  for(i=0;i<8;i++) {
    printf("D%d: %08X %-20s A%d: %08X\n", i, cpu->internal->d[i], "", i, cpu->internal->a[i]);
  }
  printf("PC: %08X\n", cpu->internal->pc);
  printf("SR: %04X\n", cpu->internal->sr);
  printf("SSP: %08X\n", cpu->internal->ssp);
  printf("USP: %08X\n", cpu->internal->usp);
  printf("Prefetch: %04X %04X %04X\n", cpu->internal->irc, cpu->internal->ir, cpu->internal->ird);
  printf("Cycles: %ld\n", cpu->internal->cycles);
  printf("ICycle: %d (%d)\n", cpu->internal->icycle, cpu->exec->cycles);
  printf("Current uOP: %s (%d)\n", uops_names[cpu->exec->instr->uops_types[cpu->exec->uops_pos]], cpu->exec->uops_pos);
  printf("State: %s\n", states[cpu->internal->main_state]);
  printf("External: %08x %04x %d\n", cpu->external->address, cpu->external->data, cpu->external->data_available);
  printf("\n");
}

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

  /* The BOOT instruction does not really exist, but is a special case for
   * the power on state of the machine, where SP, PC and an extra Prefetch
   * step is done.
   */
  cpu->exec->instr = instr_boot_setup(cpu);
  cpu->exec->uops_pos = 0;
  cpu->exec->cycles = 0;
  cpu->external->data_available = 0;

  cpu->hws = hws;

  instr_nop_setup(cpu);
  
  return cpu;
}

void cpu_initiate_next_instruction(struct cpu *cpu) {
  cpu->exec->instr = cpu->internal->instr[cpu->internal->ird];
  printf("DEBUG: New instruction: %04X %p\n", cpu->internal->ird, cpu->exec->instr);
  cpu->exec->uops_pos = 0;
  cpu->exec->cycles = 0;
  cpu->external->data_available = 0;
}

void cpu_step_instr(struct cpu *cpu) {
  int pos;

  /* uOPs all consume 2 cycles */
  if((cpu->internal->cycles & 1) == 0) {
    pos = cpu->exec->uops_pos;
    /* Check if previous instruction has ended */
    if(cpu->exec->instr->uops_types[pos] == INSTR_UOP_END) {
      cpu->internal->icycle = cpu->exec->cycles;
      cpu_initiate_next_instruction(cpu);
      pos = cpu->exec->uops_pos;
    }
    printf("DEBUG: pos == %d\n", pos);
    cpu->exec->instr->uops[pos](cpu);
  }
  cpu->internal->cycles++;
  cpu->exec->cycles++;
}

void cpu_tick(struct hw *hw) {
  //  struct cpu *cpu;
  //  cpu = (struct cpu *)hw->data;
  cpu_step_instr(hw->data);
  cpu_debug_info(hw->data);
}

void cpu_instr_register(struct cpu *cpu, WORD op, WORD op_mask, struct instr *instr) {
  int i;
  for(i=0;i<65536;i++) {
    if((i&op_mask) == (op&op_mask)) {
      cpu->internal->instr[i] = instr;
    }
  }
}

