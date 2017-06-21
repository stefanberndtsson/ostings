#include "instr.h"
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* Address from (An). Just copy the address of the An into the target_reg
 * then prefetch a new WORD.
 */
static void ea_addr_mem(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
}

/* Address from (An)+. Put An into target_reg, then increment An as required
 * followed by a prefetch.
 */
static void ea_addr_mem_inc(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_inc_reg(instr, REG_AREG(ea_reg), size);
  instr_uop_push_prefetch(instr);
}

/* Address from -(An). Decrement An first, then copy to target_reg,
 * and finally a prefetch.
 */
static void ea_addr_mem_dec(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_nop(instr);
  instr_uop_push_dec_reg(instr, REG_AREG(ea_reg), size);
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
}

/* Address from d16(An). Copy An to target_reg, 
 * d16 is already in IRC, so add that to target_reg,
 * then another prefetch.
 */
static void ea_addr_mem_offset(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_add_word_to_long(instr, REG_IRC_W, target_reg);
  instr_uop_push_nop(instr);
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

/* Address from d16(PC). Identical to d16(An), except
 * PC has moved after prefetching, so target_reg needs to
 * be decremented by one WORD.
 * Finish off by prefetching.
 */
static void ea_addr_pc_offset(struct instr *instr, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_PC, target_reg);
  instr_uop_push_add_word_to_long(instr, REG_IRC_W, target_reg);
  instr_uop_push_dec_reg(instr, target_reg, INSTR_WORD);
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
 * After the first prefetch, the full value is in IRD (High WORD)
 * and IRC (Low WORD), so just copy those two into target_reg,
 * then do the last prefetch.
 */
static void ea_addr_long(struct instr *instr, LONG target_reg) {
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_reg_copy_word(instr, REG_IRD_W, REG_WORD_HIGH(target_reg));
  instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_LOW(target_reg));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
}

/* Before doing anything, the first WORD (High in case of .L, Low in case of .W)
 * is already in IRC. Copy that first into its appropriate place, then prefetch.
 * If .W, it's done, otherwise, repeat for the Low WORD of the .L operation.
 */
void ea_read_immediate(struct instr *instr, LONG target_reg, enum instr_sizes size) {
  if(size == INSTR_LONG) {
    instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_HIGH(target_reg));
  } else {
    instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_LOW(target_reg));
  }
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  if(size == INSTR_LONG) {
    instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_LOW(target_reg));
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_prefetch(instr);
  }
}

static void ea_addr_generic(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
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

void ea_addr(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
  /* Do not jump before last prefetch */
  ea_addr_generic(instr, ea_mode, ea_reg, size, target_reg);
}

void ea_read_from_addr(struct instr *instr, enum instr_sizes size, LONG address_value_reg, LONG target_reg) {
  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_read_word(instr, address_value_reg, REG_WORD_HIGH(target_reg));
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_read_next_word(instr, address_value_reg, REG_WORD_LOW(target_reg));
  } else {
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_read_word(instr, address_value_reg, REG_WORD_LOW(target_reg));
  }
}

/* Destroys cpu->internal->r.value[7] */
void ea_read(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
  ea_addr(instr, ea_mode, ea_reg, size, REG_VALUE(7));
  ea_read_from_addr(instr, size, REG_VALUE(7), target_reg);
}
