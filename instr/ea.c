#include "instr.h"
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* Address from (An). Just copy the address of the An into the target_reg */
static void ea_addr_mem(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
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
                      REG_AREG(ea_reg), target_reg, size, EXT_NONE);
  instr_uop_push_prefetch(instr);
}

/* Address from -(An). This is easier, because there is an extra uOP for decrement available */
static void ea_addr_mem_dec(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_predec_reg(instr, REG_AREG(ea_reg), size);
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_prefetch(instr);
}

/* Address from d16(An). Helper to copy and then increment. */
static void ea_uop_add_irc_to_ea_reg(struct uop *uop, struct cpu *cpu) {
  LONG target_value;
  WORD offset;

  offset = SIGN_EXT_WORD(cpu->internal->r.irc);
  target_value = cpu->internal->r.a[uop->data1];
  printf("DEBUG: UOP-helper: %08X\n", offset);
  printf("DEBUG: UOP-helper: %08X\n", target_value);
  printf("DEBUG: UOP-helper: target-reg: %d\n", uop->data2);

  target_value += offset;
  
  cpu->internal->l[uop->data2] = target_value;
  cpu->exec->uops_pos++;
}

/* Address from d16(An). Copy An to target_reg, fetch d16 from prefetch, add d16 to target_reg */
static void ea_addr_mem_offset(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_full(instr, ea_uop_add_irc_to_ea_reg, INSTR_UOP_EA_SPECIAL,
                      ea_reg, target_reg, INSTR_WORD, EXT_NONE);
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

/* Address from d16(PC). Helper to copy and then increment, subtract two, because of an earlier prefetch. */
static void ea_uop_add_irc_to_pc(struct uop *uop, struct cpu *cpu) {
  LONG target_value;
  WORD offset;

  offset = SIGN_EXT_WORD(cpu->internal->r.irc);
  target_value = cpu->internal->r.pc;
  printf("DEBUG: UOP-PC-helper: %08X\n", offset);
  printf("DEBUG: UOP-PC-helper: %08X\n", target_value);
  printf("DEBUG: UOP-PC-helper: target-reg: %d\n", uop->data1);

  target_value += offset - 2;
  
  cpu->internal->l[uop->data1] = target_value;
  cpu->exec->uops_pos++;
}

/* Address from d16(PC). Copy PC to target_reg, fetch d16 from prefetch, add d16 to target_reg */
static void ea_addr_pc_offset(struct instr *instr, LONG target_reg) {
  instr_uop_push_full(instr, ea_uop_add_irc_to_pc, INSTR_UOP_EA_SPECIAL,
                      target_reg, 0, INSTR_WORD, EXT_NONE);
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
 */
static void ea_addr_long(struct instr *instr, LONG target_reg) {
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_into(instr, REG_WORD_HIGH(target_reg));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch_next_into(instr, REG_WORD_LOW(target_reg));
}

void ea_read_immediate(struct instr *instr, LONG reg_num, enum instr_sizes size) {
  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch_into(instr, REG_WORD_HIGH(reg_num));
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch_next_into(instr, REG_WORD_LOW(reg_num));
  } else {
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch_into(instr, REG_WORD_LOW(reg_num));
  }
}

void ea_addr(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
  switch(ea_mode) {
  case EA_MEM:
    ea_addr_mem(instr, ea_reg, target_reg);
    break;
  case EA_MEM_INC:
    ea_addr_mem_inc(instr, ea_reg, size, target_reg);
    break;
  case EA_MEM_DEC:
    ea_addr_mem_dec(instr, ea_reg, size, target_reg);
    break;
  case EA_MEM_OFFSET:
    ea_addr_mem_offset(instr, ea_reg, target_reg);
    break;
  case EA_MEM_OFFSET_REG:
    ea_addr_mem_offset_reg(instr, ea_reg, size, target_reg);
    break;
  case EA_EXTENDED:
    switch(ea_reg) {
    case EA_SHORT:
      ea_addr_short(instr, target_reg);
      break;
    case EA_LONG:
      ea_addr_long(instr, target_reg);
      break;
    case EA_PC_OFFSET:
      ea_addr_pc_offset(instr, target_reg);
      break;
    case EA_PC_OFFSET_REG:
      ea_addr_pc_offset_reg(instr, size, target_reg);
      break;
    }
  }
  /* TODO: Better error handling! */
  return;
}

void ea_read_from_addr(struct instr *instr, enum instr_sizes size, LONG address_value_reg, LONG target_reg) {
  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_read_word(instr, address_value_reg, REG_WORD_HIGH(target_reg));
    instr_uop_push_nop(instr);
    instr_uop_push_read_next_word(instr, address_value_reg, REG_WORD_LOW(target_reg));
  } else {
    instr_uop_push_nop(instr);
    instr_uop_push_read_word(instr, address_value_reg, REG_WORD_LOW(target_reg));
  }
}

/* Destroys cpu->internal->r.value[7] */
void ea_read(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
  /* ea_addr_long differs from the others due to it reading two words, not one for the address */
  ea_addr(instr, ea_mode, ea_reg, size, REG_VALUE(7));
  ea_read_from_addr(instr, size, REG_VALUE(7), target_reg);
}
