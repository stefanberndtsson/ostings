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
    cpu->external->address = cpu->internal->r.pc;
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

void uop_prefetch_generic(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->address = cpu->internal->r.pc;
    cpu->internal->r.ir = cpu->internal->r.irc;
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->r.pc += 2;
    cpu->internal->r.irc = cpu->external->data;
    cpu->internal->r.ird = cpu->internal->r.ir;
    if(uop->code == INSTR_UOP_PREFETCH_INTO) {
      cpu->internal->w[uop->data1] = cpu->internal->r.ird;
    } else if(uop->code == INSTR_UOP_PREFETCH_NEXT_INTO) {
      cpu->internal->w[uop->data1] = cpu->internal->r.ird;
    }
    mmu_clear_read_progress(cpu->mmu);
    cpu->exec->uops_pos++;
  }
}

/* Prefetch cycle:
 * Copy IRC to IR
 * Fetch new WORD to IRC
 * Copy IR to IRD
 */
void uop_prefetch(struct uop *uop, struct cpu *cpu) {
  uop_prefetch_generic(uop, cpu);
}


/* Prefetch like normal, but also store WORD ending up in IRD
 * into another register
 */
void uop_prefetch_into(struct uop *uop, struct cpu *cpu) {
  uop_prefetch_generic(uop, cpu);
}


/* Similar to READ_WORD_NEXT, this is for fetching the second
 * half of a LONG into (hopefully) the same register
 *
 * TODO: Handle BIG_ENDIAN
 */
void uop_prefetch_next_into(struct uop *uop, struct cpu *cpu) {
  uop_prefetch_generic(uop, cpu);
}


/* TODO: Sign extend? */
void uop_read_word(struct uop *uop, struct cpu *cpu) {
  if(!cpu->mmu->read_in_progress) {
    cpu->external->data_available = 0;
    cpu->external->address = cpu->internal->l[uop->data1];
    mmu_read_word(cpu->mmu);
  }
  if(cpu->external->data_available) {
    cpu->internal->w[uop->data2] = cpu->external->data;
    mmu_clear_read_progress(cpu->mmu);
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
    cpu->internal->w[uop->data2] = cpu->external->data;
    mmu_clear_read_progress(cpu->mmu);
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

void uop_predec_reg(struct uop *uop, struct cpu *cpu) {
  if(uop->size == INSTR_BYTE) { cpu->internal->l[uop->data1] -= 1; }
  if(uop->size == INSTR_WORD) { cpu->internal->l[uop->data1] -= 2; }
  if(uop->size == INSTR_LONG) { cpu->internal->l[uop->data1] -= 4; }
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
void uop_postinc_reg(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented (should never be called): %s\n", "uop_postinc_reg");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_read_byte(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_read_byte");
  exit(-100);
}

/* TODO: Unimplemented */
void uop_write_byte(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  unused(cpu);
  printf("DEBUG-UOP: Unimplemented: %s\n", "uop_write_byte");
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

