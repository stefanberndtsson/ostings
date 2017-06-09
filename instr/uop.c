#include "instr.h"
#include "mmu.h"

void uop_unop(struct cpu *cpu) {
  cpu->exec->uops_pos++;
  return;
}

void uop_boot_prefetch(struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->pc;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->irc = cpu->external->data;
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_prog_read(struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->pc;
    cpu->internal->ir = cpu->internal->irc;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->pc += 2;
    cpu->internal->irc = cpu->external->data;
    cpu->internal->ird = cpu->internal->ir;
    cpu->exec->uops_pos++;
  }

  return;
}

void uop_read_word(struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_end(struct cpu *cpu) {
  return;
}
