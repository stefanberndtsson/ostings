#include "instr.h"
#include "mmu.h"

void uop_unop(struct cpu *cpu) {
  cpu->exec->uops_pos++;
  return;
}

void uop_boot_prefetch(struct cpu *cpu) {
  if(!cpu->exec->flag) {
    cpu->external->address = cpu->internal->pc;
    mmu_read_word(cpu->mmu);
    cpu->exec->flag = 1;
  }
  if(cpu->external->data_available) {
    cpu->internal->irc = cpu->external->data;
    cpu->exec->flag = 0;
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_prog_read(struct cpu *cpu) {
  if(!cpu->exec->flag) {
    cpu->external->address = cpu->internal->pc;
    cpu->internal->ir = cpu->internal->irc;
    mmu_read_word(cpu->mmu);
    cpu->exec->flag = 1;
  }
  if(cpu->external->data_available) {
    cpu->internal->pc += 2;
    cpu->internal->irc = cpu->external->data;
    cpu->internal->ird = cpu->internal->ir;
    cpu->exec->flag = 0;
    cpu->exec->uops_pos++;
  }

  return;
}

void uop_read_word(struct cpu *cpu) {
  if(!cpu->exec->flag) {
    cpu->external->data_available = 0;
    mmu_read_word(cpu->mmu);
    cpu->exec->flag = 1;
  }
  if(cpu->external->data_available) {
    cpu->exec->flag = 0;
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_end(struct cpu *cpu) {
  return;
}
