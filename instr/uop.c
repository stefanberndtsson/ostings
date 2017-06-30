#include "common.h"
#include "instr.h"
#include "mmu.h"

/* Do nothing for one period */
void uop_nop(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  cpu->exec->uops_pos++;
  return;
}

/* Do nothing multiple periods */
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

/* Only executed at boot. Prefetch a WORD to IRC */
void uop_boot_prefetch(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->r.pc&0xffffff;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->r.pc += 2;
    cpu->internal->r.irc = cpu->external->data;
    cpu->exec->uops_pos++;
    mmu_clear_read_progress(cpu->mmu);
  }
  return;
}

/* Prefetch cycle:
 * Copy IRC to IR
 * Fetch new WORD to IRC
 * Copy IR to IRD
 */
void uop_prefetch(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->r.pc&0xffffff;
    cpu->internal->r.ir = cpu->internal->r.irc;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    /* Waitstate resolve can only occur on odd cycles */
    if(!cpu->mmu->read_in_progress || (cpu->mmu->read_in_progress && (cpu->exec->cycles%2 == 1))) {
      cpu->internal->r.pc += 2;
      cpu->internal->r.irc = cpu->external->data;
      cpu->internal->r.ird = cpu->internal->r.ir;
      mmu_clear_read_progress(cpu->mmu);
      cpu->exec->uops_pos++;
    }
  }
}


/* TODO: Sign extend? */
void uop_read_byte(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->internal->l[uop->data1]&0xffffff;
    mmu_read_byte(cpu->mmu);
  }
  if(cpu->external->data_available) {
    /* Waitstate resolve can only occur on odd cycles */
    if(!cpu->mmu->read_in_progress || (cpu->mmu->read_in_progress && (cpu->exec->cycles%2 == 1))) {
      cpu->internal->w[uop->data2] = (cpu->internal->w[uop->data2]&0xff00) | (cpu->external->data&0xff);
      mmu_clear_read_progress(cpu->mmu);
      cpu->exec->uops_pos++;
    }
  }
  return;
}


/* TODO: Sign extend? */
void uop_read_word(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->internal->l[uop->data1]&0xffffff;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    /* Waitstate resolve can only occur on odd cycles */
    if(!cpu->mmu->read_in_progress || (cpu->mmu->read_in_progress && (cpu->exec->cycles%2 == 1))) {
      cpu->internal->w[uop->data2] = cpu->external->data;
      mmu_clear_read_progress(cpu->mmu);
      cpu->exec->uops_pos++;
    }
  }
  return;
}


/* TODO: Properly handle BIG_ENDIAN */
void uop_read_next_word(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = (cpu->internal->l[uop->data1] + 2)&0xffffff;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    /* Waitstate resolve can only occur on odd cycles */
    if(!cpu->mmu->read_in_progress || (cpu->mmu->read_in_progress && (cpu->exec->cycles%2 == 1))) {
      cpu->internal->w[uop->data2] = cpu->external->data;
      mmu_clear_read_progress(cpu->mmu);
      cpu->exec->uops_pos++;
    }
  }
  return;
}


/* TODO: Sign extend? */
void uop_write_byte(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->write_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->internal->l[uop->data1]&0xffffff;
    cpu->external->data = cpu->internal->w[uop->data2];
    mmu_write_byte(cpu->mmu);
    printf("DEBUG-UOP-WRITE: Initiating write at %d\n", cpu->exec->cycles);
  }
  if(cpu->external->data_available) {
    /* Waitstate resolve can only occur on odd cycles */
    if(!cpu->mmu->write_in_progress || (cpu->mmu->write_in_progress && (cpu->exec->cycles%2 == 1))) {
      mmu_clear_write_progress(cpu->mmu);
      cpu->exec->uops_pos++;
      printf("DEBUG-UOP-WRITE: Actually wrote data at %d\n", cpu->exec->cycles);
    } else {
      printf("DEBUG-UOP-WRITE: Could not write data at %d, progress %d\n", cpu->exec->cycles, cpu->mmu->write_in_progress);
    }
  }
  return;
}


/* TODO: Sign extend */
void uop_reg_copy(struct uop *uop, struct cpu *cpu) {
  if(uop->size == INSTR_LONG) {
    cpu->internal->l[uop->data2] = cpu->internal->l[uop->data1];
  } else if(uop->size == INSTR_WORD) {
    cpu->internal->w[uop->data2] = cpu->internal->w[uop->data1];
  } else if(uop->size == INSTR_BYTE) {
    cpu->internal->w[uop->data2] = (cpu->internal->w[uop->data2]&0xff00)|(cpu->internal->w[uop->data1]&0xff);
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

void uop_dec_reg(struct uop *uop, struct cpu *cpu) {
  if(uop->size == INSTR_BYTE) { cpu->internal->l[uop->data1] -= 1; }
  if(uop->size == INSTR_WORD) { cpu->internal->l[uop->data1] -= 2; }
  if(uop->size == INSTR_LONG) { cpu->internal->l[uop->data1] -= 4; }
  cpu->exec->uops_pos++;
}


void uop_inc_reg(struct uop *uop, struct cpu *cpu) {
  if(uop->size == INSTR_BYTE) { cpu->internal->l[uop->data1] += 1; }
  if(uop->size == INSTR_WORD) { cpu->internal->l[uop->data1] += 2; }
  if(uop->size == INSTR_LONG) { cpu->internal->l[uop->data1] += 4; }
  cpu->exec->uops_pos++;
}

void uop_add(struct uop *uop, struct cpu *cpu) {
  LONG src_reg, target_reg;
  enum instr_sizes target_size;

  /* src_reg size is uop->size */
  /* target_reg size is uop->size, unless
   * uop->ext is EXT_WORD or EXT_LONG,
   * in which case target_reg size is either
   * WORD or LONG respectively.
   */

  if(uop->size == INSTR_BYTE) {
    src_reg = cpu->internal->w[uop->data1]&0xff;
    if(uop->ext == EXT_LONG) {
      src_reg = SIGN_EXT_WORD(SIGN_EXT_BYTE(src_reg));
    } else if(uop->ext == EXT_WORD) {
      src_reg = SIGN_EXT_BYTE(src_reg);
    }
  } else if(uop->size == INSTR_WORD) {
    src_reg = cpu->internal->w[uop->data1];
    if(uop->ext == EXT_LONG) {
      src_reg = SIGN_EXT_WORD(src_reg);
    }
  } else if(uop->size == INSTR_LONG) {
    src_reg = cpu->internal->l[uop->data1];
  }

  if(uop->ext == EXT_LONG) {
    target_reg = cpu->internal->l[uop->data2];
    target_size = INSTR_LONG;
  } else if(uop->ext == EXT_WORD) {
    target_reg = cpu->internal->w[uop->data2]&0xffff;
    target_size = INSTR_WORD;
  } else if(uop->ext == EXT_NONE) {
    if(uop->size == INSTR_BYTE) {
      target_reg = cpu->internal->w[uop->data2]&0xff;
      target_size = INSTR_BYTE;
    } else if(uop->size == INSTR_WORD) {
      target_reg = cpu->internal->w[uop->data2];
      target_size = INSTR_WORD;
    } else if(uop->size == INSTR_LONG) {
      target_reg = cpu->internal->l[uop->data2];
      target_size = INSTR_LONG;
    }
  }

  target_reg += src_reg;

  if(target_size == INSTR_BYTE) {
    cpu->internal->w[uop->data2] = (cpu->internal->w[uop->data2]&0xff00) | (target_reg&0xff);
  } else if(target_size == INSTR_WORD) {
    cpu->internal->w[uop->data2] = target_reg&0xffff;
  } else if(target_size == INSTR_LONG) {
    cpu->internal->l[uop->data2] = target_reg;
  }
  cpu->exec->uops_pos++;
}

void uop_reg_copy_ext_to_word(struct uop *uop, struct cpu *cpu) {
  WORD value;
  value = cpu->internal->w[uop->data1];

  /* Only BYTE really makes sense here, do nothing otherwise */
  if(uop->size == INSTR_BYTE) {
    value = SIGN_EXT_BYTE(value&0xff);
  }
  cpu->internal->w[uop->data2] = value;
  cpu->exec->uops_pos++;
}

void uop_reg_copy_ext_to_long(struct uop *uop, struct cpu *cpu) {
  LONG value;
  /* reg1 is of WORD/BYTE size, anything else is silly */
  value = cpu->internal->w[uop->data1];

  if(uop->size == INSTR_BYTE) {
    value = SIGN_EXT_WORD(SIGN_EXT_BYTE(value&0xff));
  } else if(uop->size == INSTR_WORD) {
    value = SIGN_EXT_WORD(value&0xffff);
  }
  cpu->internal->l[uop->data2] = value;
  cpu->exec->uops_pos++;
}

/* Set ZN according to value, clear V and C */
void uop_set_basic_flags(struct uop *uop, struct cpu *cpu) {
  LONG value;
  int n = 0;
  int z = 0;
  int v = 0;
  int c = 0;
  
  if(uop->size == INSTR_BYTE) {
    value = cpu->internal->w[uop->data1];
    if(value == 0) { z = 1; }
    if(value&0x80) { n = 1; }
  } else if(uop->size == INSTR_WORD) {
    value = cpu->internal->w[uop->data1];
    if(value == 0) { z = 1; }
    if(value&0x8000) { n = 1; }
  } else if(uop->size == INSTR_LONG) {
    value = cpu->internal->l[uop->data1];
    if(value == 0) { z = 1; }
    if(value&0x80000000) { n = 1; }
  }
  SET_NZVC(cpu, n, z, v, c);
  cpu->exec->uops_pos++;
}


void uop_end(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  return;
}


/* TODO: Unimplemented */
void uop_special(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_special");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_ea_special(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_ea_special");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_write_word(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_write_word");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_write_next_word(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_write_next_word");
  exit(-100);
}

