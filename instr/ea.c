#include "instr.h"

/* OBS! Note that value_num is one of 0-7 denoting the cpu->internal->r.value[], not a cpu->internal->w reg num 
 * This is because this is a temporary value used for fetching the address, and it will be irrelevant once
 * the value from it has been read. It's also because I'm too lazy to handle the difference between
 * numbering of cpu->internal->w (used by prefetch), and cpu->internal->l (used by read_word) for the
 * same value-register.
 */
void ea_read_abs_long(struct instr *instr, LONG intermediate_value_num, LONG value_reg_num, enum instr_sizes size) {
  /* Fetch LONG address from prefetch queue */
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_into(instr, REG_VALUE_H_TO_REG_W(intermediate_value_num));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_next_into(instr, REG_VALUE_H_TO_REG_W(intermediate_value_num));

  /* Now fetch value of right size from that address */
  instr_uop_push_nop(instr);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, REG_VALUE_TO_REG_L(intermediate_value_num), value_reg_num, size, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, REG_VALUE_TO_REG_L(intermediate_value_num), value_reg_num, size, EXT_NONE);
}

void ea_read_immediate(struct instr *instr, LONG reg_num, enum instr_sizes size) {
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_into(instr, reg_num);
  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch_next_into(instr, reg_num);
  }
}

