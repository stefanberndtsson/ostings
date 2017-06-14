#include "instr.h"
#include "mmu.h"

void uop_unop(struct cpu *cpu, LONG data) {
  cpu->exec->uops_pos++;
  return;
}

void uop_boot_prefetch(struct cpu *cpu, LONG data) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->r.pc;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->r.pc += 2;
    cpu->internal->r.irc = cpu->external->data;
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_prog_read(struct cpu *cpu, LONG data) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->r.pc;
    cpu->internal->r.ir = cpu->internal->r.irc;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->r.pc += 2;
    cpu->internal->r.irc = cpu->external->data;
    cpu->internal->r.ird = cpu->internal->r.ir;
    cpu->exec->uops_pos++;
  }
  
  return;
}

void uop_ird_to_value_low(struct cpu *cpu, LONG value_num) {
  cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff0000) | cpu->internal->r.ird;
  cpu->exec->uops_pos++;
}

void uop_ird_to_value_high(struct cpu *cpu, LONG value_num) {
  cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff) | (cpu->internal->r.ird << 16);
  cpu->exec->uops_pos++;
}

void uop_data_to_value_low(struct cpu *cpu, LONG value_num) {
  cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff0000) | cpu->external->data;
  cpu->exec->uops_pos++;
}

void uop_data_to_value_high(struct cpu *cpu, LONG value_num) {
  cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff) | (cpu->external->data << 16);
  cpu->exec->uops_pos++;
}

/* TODO: Clean up this bit with value0-2 */
//void uop_assemble_value0(struct cpu *cpu) {
//  printf("DEBUG-UOP: assemble-0: %04X %04X\n", cpu->internal->irc, cpu->internal->ird);
//  cpu->exec->value0 <<= 16;
//  cpu->exec->value0 |= cpu->internal->ird;
//  cpu->exec->uops_pos++;
//}

//void uop_assemble_value1(struct cpu *cpu) {
//  printf("DEBUG-UOP: assemble-1: %04X %04X\n", cpu->internal->irc, cpu->internal->ird);
//  cpu->exec->value1 <<= 16;
//  cpu->exec->value1 |= cpu->internal->ird;
//  cpu->exec->uops_pos++;
//}

//void uop_assemble_value2(struct cpu *cpu) {
//  printf("DEBUG-UOP: assemble-2: %04X %04X\n", cpu->internal->irc, cpu->internal->ird);
//  cpu->exec->value2 <<= 16;
//  cpu->exec->value2 |= cpu->internal->ird;
//  cpu->exec->uops_pos++;
//}

void uop_read_word(struct cpu *cpu, LONG value_num) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->exec->value[value_num];
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_read_next_word(struct cpu *cpu, LONG value_num) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->exec->value[value_num] + 2;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_end(struct cpu *cpu, LONG data) {
  return;
}


/* TODO: Unimplemented */
void uop_spec(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_spec");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_unopcnt(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_unopcnt");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_read_byte(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_read_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_write_byte(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_write_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_write_word(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_write_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_write_next_word(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_write_next_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value_low_to_data(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value_low_to_data");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value_high_to_data(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value_high_to_data");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_swap(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_swap");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_dreg_byte(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_dreg_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_dreg_word(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_dreg_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_dreg_long(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_dreg_long");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_dreg_byte_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_dreg_byte_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_dreg_word_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_dreg_word_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_areg_byte(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_areg_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_areg_word(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_areg_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_areg_long(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_areg_long");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_areg_byte_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_areg_byte_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_value0_to_areg_word_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_value0_to_areg_word_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_dreg_to_value0_byte(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_dreg_to_value0_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_dreg_to_value0_word(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_dreg_to_value0_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_dreg_to_value0_long(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_dreg_to_value0_long");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_dreg_to_value0_byte_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_dreg_to_value0_byte_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_dreg_to_value0_word_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_dreg_to_value0_word_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_areg_to_value0_byte(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_areg_to_value0_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_areg_to_value0_word(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_areg_to_value0_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_areg_to_value0_long(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_areg_to_value0_long");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_areg_to_value0_byte_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_areg_to_value0_byte_sext");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_areg_to_value0_word_sext(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_areg_to_value0_word_sext");
  exit(-100);
}
