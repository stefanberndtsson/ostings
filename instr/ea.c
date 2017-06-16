#include "instr.h"
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* Address from (An). Just copy the address of the An into the target_reg */
static void ea_addr_mem(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG_TO_REG_L(ea_reg), target_reg);
  instr_uop_push_prefetch(instr);
}

/* Address from (An)+. Helper to copy and then increment. */
static void ea_uop_reg_copy_and_inc(struct uop *uop, struct cpu *cpu) {
  LONG value;

  value = cpu->internal->l[uop->data1];
  cpu->internal->l[uop->data2] = value;
  if(uop->size == INSTR_BYTE) { value += 1; }
  if(uop->size == INSTR_WORD) { value += 2; }
  if(uop->size == INSTR_LONG) { value += 4; }

  cpu->internal->l[uop->data1] = value;
  cpu->exec->uops_pos++;
}

/* Address from (An)+. Put An into target_reg, then increment An as required */
static void ea_addr_mem_inc(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_full(instr, ea_uop_reg_copy_and_inc, INSTR_UOP_EA_SPECIAL,
                      REG_AREG_TO_REG_L(ea_reg), target_reg, size, EXT_NONE);
  instr_uop_push_prefetch(instr);
}

/* Address from -(An). This is easier, because there is an extra uOP for decrement available */
static void ea_addr_mem_dec(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_predec_reg(instr, REG_AREG_TO_REG_L(ea_reg), size);
  instr_uop_push_reg_copy_long(instr, REG_AREG_TO_REG_L(ea_reg), target_reg);
  instr_uop_push_prefetch(instr);
}

/* Address from d16(An) or d16(PC). Helper to copy and then increment. */
static void ea_uop_add_word_to_addr(struct uop *uop, struct cpu *cpu) {
  LONG target_value;
  WORD offset;

  offset = SIGN_EXT_WORD(cpu->internal->w[uop->data1]);
  target_value = cpu->internal->l[uop->data2];

  target_value += offset;
  
  cpu->internal->l[uop->data2] = target_value;
  cpu->exec->uops_pos++;
}

/* Address from d16(An). Copy An to target_reg, fetch d16 from prefetch, add d16 to target_reg */
static void ea_addr_mem_offset(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG_TO_REG_L(ea_reg), target_reg);
  instr_uop_push_prefetch(instr);
  instr_uop_push_full(instr, ea_uop_add_word_to_addr, INSTR_UOP_EA_SPECIAL,
                      REG_IRD_TO_REG_W, target_reg, INSTR_WORD, EXT_NONE);
  instr_uop_push_prefetch(instr);
}

/* Address from d8(An,Rn.S).
 * TODO: Unimplemented.
 */
static void ea_addr_mem_offset_reg(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  unused(instr);
  unused(ea_reg);
  unused(size);
  unused(target_reg);
}

/* Address from d16(PC). Copy PC to target_reg, fetch d16 from prefetch, add d16 to target_reg */
static void ea_addr_pc_offset(struct instr *instr, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_PC_TO_REG_L, target_reg);
  instr_uop_push_prefetch(instr);
  instr_uop_push_full(instr, ea_uop_add_word_to_addr, INSTR_UOP_EA_SPECIAL,
                      REG_IRD_TO_REG_W, target_reg, INSTR_WORD, EXT_NONE);
  instr_uop_push_prefetch(instr);
}

/* Address from d8(PC,Rn.S).
 * TODO: Unimplemented.
 */
static void ea_addr_pc_offset_reg(struct instr *instr, enum instr_sizes size, LONG target_reg) {
  unused(instr);
  unused(size);
  unused(target_reg);
}

/* Address from $xxxx.W
 * TODO: Unimplemented.
 */
static void ea_addr_short(struct instr *instr, LONG target_reg) {
  unused(instr);
  unused(target_reg);
}

/* Address from $xxxxxxxx.L
 * TODO: Unimplemented.
 */
static void ea_addr_long(struct instr *instr, LONG target_reg) {
  unused(instr);
  unused(target_reg);
}

/* OBS! Note that value_num is one of 0-7 denoting the cpu->internal->r.value[], not a cpu->internal->w reg num 
 * This is because this is a temporary value used for fetching the address, and it will be irrelevant once
 * the value from it has been read. It's also because I'm too lazy to handle the difference between
 * numbering of cpu->internal->w (used by prefetch), and cpu->internal->l (used by read_word) for the
 * same value-register.
 */
void ea_read_abs_long(struct instr *instr, LONG intermediate_value_num, LONG value_reg_num, enum instr_sizes size) {
  unused(size);
  
  /* Fetch LONG address from prefetch queue */
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_into(instr, REG_VALUE_H_TO_REG_W(intermediate_value_num));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_next_into(instr, REG_VALUE_H_TO_REG_W(intermediate_value_num));

  /* Now fetch value of right size from that address */
  instr_uop_push_nop(instr);
  instr_uop_push_read_word(instr, REG_VALUE_TO_REG_L(intermediate_value_num), value_reg_num);
  instr_uop_push_nop(instr);
  instr_uop_push_read_next_word(instr, REG_VALUE_TO_REG_L(intermediate_value_num), value_reg_num);
}

void ea_read_immediate(struct instr *instr, LONG reg_num, enum instr_sizes size) {
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_into(instr, reg_num);
  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch_next_into(instr, reg_num);
  }
}

void ea_addr(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
  switch(ea_mode) {
  case EA_MEM:
    ea_addr_mem(instr, ea_reg, target_reg);
  case EA_MEM_INC:
    ea_addr_mem_inc(instr, ea_reg, size, target_reg);
  case EA_MEM_DEC:
    ea_addr_mem_dec(instr, ea_reg, size, target_reg);
  case EA_MEM_OFFSET:
    ea_addr_mem_offset(instr, ea_reg, target_reg);
  case EA_MEM_OFFSET_REG:
    ea_addr_mem_offset_reg(instr, ea_reg, size, target_reg);
  case EA_EXTENDED:
    switch(ea_reg) {
    case EA_SHORT:
      ea_addr_short(instr, target_reg);
    case EA_LONG:
      ea_addr_long(instr, target_reg);
    case EA_PC_OFFSET:
      ea_addr_pc_offset(instr, target_reg);
    case EA_PC_OFFSET_REG:
      ea_addr_pc_offset_reg(instr, size, target_reg);
    }
  }
  /* TODO: Better error handling! */
  return;
}
