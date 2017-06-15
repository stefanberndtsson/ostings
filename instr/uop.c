#include "common.h"
#include "instr.h"
#include "mmu.h"

void uop_nop(struct uop *uop, struct cpu *cpu) {
  cpu->exec->uops_pos++;
  return;
}

void uop_nopcnt(struct uop *uop, struct cpu *cpu) {
  if(cpu->exec->counter == -1) {
    cpu->exec->counter = uop->data1;
  }

  if(cpu->exec->counter > 0) {
    cpu->exec->counter--;
  } else {
    cpu->exec->uops_pos++;
  }
  return;
}

void uop_boot_prefetch(struct uop *uop, struct cpu *cpu) {
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

void uop_prefetch(struct uop *uop, struct cpu *cpu) {
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

// void uop_ird_to_value_low(struct cpu *cpu, LONG value_num) {
//   cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff0000) | cpu->internal->r.ird;
//   cpu->exec->uops_pos++;
// }
// 
// void uop_ird_to_value_high(struct cpu *cpu, LONG value_num) {
//   cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff) | (cpu->internal->r.ird << 16);
//   cpu->exec->uops_pos++;
// }
// 
// void uop_data_to_value_low(struct cpu *cpu, LONG value_num) {
//   cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff0000) | cpu->external->data;
//   cpu->exec->uops_pos++;
// }
// 
// void uop_data_to_value_high(struct cpu *cpu, LONG value_num) {
//   cpu->exec->value[value_num] = (cpu->exec->value[value_num]&0xffff) | (cpu->external->data << 16);
//   cpu->exec->uops_pos++;
// }
// 
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

/* TODO: Sign extend? */
void uop_read_word(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->internal->l[uop->data1];
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->w[uop->data2] = cpu->external->data;
    cpu->exec->uops_pos++;
  }
  return;
}

/* TODO: Properly handle BIG_ENDIAN */
void uop_read_next_word(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->internal->l[uop->data1] + 2;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->w[uop->data2-1] = cpu->external->data;
    cpu->exec->uops_pos++;
  }
  return;
}

/* TODO: Sign extend */
void uop_reg_copy(struct uop *uop, struct cpu *cpu) {
  if(uop->size == INSTR_LONG) {
    cpu->internal->l[uop->data2] = cpu->internal->l[uop->data1];
  } else {
    cpu->internal->w[uop->data2] = cpu->internal->w[uop->data1];
  }
  cpu->exec->uops_pos++;
}

/* TODO: Sign extend */
void uop_reg_swap(struct uop *uop, struct cpu *cpu) {
  LONG tmp1;
  WORD tmp2;
  if(uop->size == INSTR_LONG) {
    tmp1 = cpu->internal->l[uop->data2];
    cpu->internal->l[uop->data2] = cpu->internal->l[uop->data1];
    cpu->internal->l[uop->data1] = tmp1;
  } else {
    tmp2 = cpu->internal->w[uop->data2];
    cpu->internal->w[uop->data2] = cpu->internal->w[uop->data1];
    cpu->internal->w[uop->data1] = tmp2;
  }
  cpu->exec->uops_pos++;
}

void uop_end(struct uop *uop, struct cpu *cpu) {
  return;
}


/* TODO: Unimplemented */
void uop_special(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_special");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_ea_special(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_ea_special");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_predec_reg(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_predec_reg");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_postinc_reg(struct cpu *cpu, LONG data) {
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_postinc_reg");
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
