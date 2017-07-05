#include "instr.h"
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* Copy Dn (of correct size) to target_reg */
static void ea_dn(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  if(size == INSTR_BYTE) {
    instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(REG_DREG(ea_reg)), REG_WORD_LOW(target_reg));
  } else if(size == INSTR_WORD) { 
    instr_uop_push_reg_copy_word(instr, REG_WORD_LOW(REG_DREG(ea_reg)), REG_WORD_LOW(target_reg));
  } else if(size == INSTR_LONG) {
    instr_uop_push_reg_copy_long(instr, REG_DREG(ea_reg), target_reg);
  }
}

/* Copy Dn (of correct size) to target_reg */
static void ea_an(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  if(size == INSTR_BYTE) {
    instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(REG_AREG(ea_reg)), REG_WORD_LOW(target_reg));
  } else if(size == INSTR_WORD) { 
    instr_uop_push_reg_copy_word(instr, REG_WORD_LOW(REG_AREG(ea_reg)), REG_WORD_LOW(target_reg));
  } else if(size == INSTR_LONG) {
    instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  }
}


/* Address from (An). Just copy the address of the An into the target_reg
 * then prefetch a new WORD.
 */
static void ea_addr_mem(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_nop(instr);
}


/* Address from (An)+. Put An into target_reg, then increment An as required
 * followed by a prefetch.
 */
static void ea_addr_mem_inc(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_inc_reg(instr, REG_AREG(ea_reg), size);
}


/* Address from -(An). Decrement An first, then copy to target_reg,
 * and finally a prefetch.
 */
static void ea_addr_mem_dec(struct instr *instr, int ea_reg, enum instr_sizes size, LONG target_reg) {
  instr_uop_push_dec_reg(instr, REG_AREG(ea_reg), size);
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_nop(instr);
}


/* Address from d16(An). Copy An to target_reg, 
 * d16 is already in IRC, so add that to target_reg,
 * then another prefetch.
 */
static void ea_addr_mem_offset(struct instr *instr, int ea_reg, LONG target_reg) {
  instr_uop_push_reg_copy_long(instr, REG_AREG(ea_reg), target_reg);
  instr_uop_push_add_word_to_long(instr, REG_IRC_W, target_reg, target_reg);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
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
  instr_uop_push_add_word_to_long(instr, REG_IRC_W, target_reg, target_reg);
  instr_uop_push_dec_reg(instr, target_reg, INSTR_WORD);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
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
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
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
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_read_next_word(instr, address_value_reg, REG_WORD_LOW(target_reg));
  } else if(size == INSTR_WORD) {
    instr_uop_push_nop(instr);
    instr_uop_push_read_word(instr, address_value_reg, REG_WORD_LOW(target_reg));
  } else if(size == INSTR_BYTE) {
    instr_uop_push_nop(instr);
    instr_uop_push_read_byte(instr, address_value_reg, REG_WORD_LOW(target_reg));
  }
}

void ea_write_to_addr(struct instr *instr, enum instr_sizes size, LONG address_value_reg, LONG source_reg) {
  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_write_word(instr, address_value_reg, REG_WORD_HIGH(source_reg));
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_write_next_word(instr, address_value_reg, REG_WORD_LOW(source_reg));
  } else if(size == INSTR_WORD) {
    instr_uop_push_nop(instr);
    instr_uop_push_write_word(instr, address_value_reg, REG_WORD_LOW(source_reg));
  } else if(size == INSTR_BYTE) {
    instr_uop_push_nop(instr);
    instr_uop_push_write_byte(instr, address_value_reg, REG_WORD_LOW(source_reg));
  }
}

/* Destroys cpu->internal->r.value[7]
 * TODO: EA_DN and EA_AN are incorrect in number of uOPs.
 */
void ea_read(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG target_reg) {
  if(ea_mode == EA_DN) {
    ea_dn(instr, ea_reg, size, target_reg);
  } else if(ea_mode == EA_AN) {
    ea_an(instr, ea_reg, size, target_reg);
  } else if(ea_mode == EA_EXTENDED && ea_reg == EA_IMMEDIATE) {
    if(size == INSTR_LONG) {
      instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_HIGH(target_reg));
      instr_uop_push_nop(instr);
      instr_uop_push_nop(instr);
      instr_uop_push_prefetch(instr);
      instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_LOW(target_reg));
      instr_uop_push_nop(instr);
      instr_uop_push_nop(instr);
      instr_uop_push_prefetch(instr);
    } else {
      instr_uop_push_reg_copy_word(instr, REG_IRC_W, REG_WORD_LOW(target_reg));
      instr_uop_push_nop(instr);
      instr_uop_push_nop(instr);
      instr_uop_push_prefetch(instr);
    }
  } else {
    ea_addr(instr, ea_mode, ea_reg, size, REG_VALUE(7));
    ea_read_from_addr(instr, size, REG_VALUE(7), target_reg);
  }
}

/* TODO: EA_DN and EA_AN are incorrect in number of uOPs */
void ea_write(struct instr *instr, int ea_mode, int ea_reg, enum instr_sizes size, LONG source_reg) {
  if(ea_mode == EA_DN) {
    if(size == INSTR_BYTE) {
      instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(source_reg), REG_WORD_LOW(REG_DREG(ea_reg)));
    } else if(size == INSTR_WORD) {
      instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(source_reg), REG_WORD_LOW(REG_DREG(ea_reg)));
    } else if(size == INSTR_LONG) {
      instr_uop_push_reg_copy_byte(instr, source_reg, REG_DREG(ea_reg));
    }
  } else if(ea_mode == EA_AN) {
    if(size == INSTR_BYTE) {
      instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(source_reg), REG_WORD_LOW(REG_AREG(ea_reg)));
    } else if(size == INSTR_WORD) {
      instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(source_reg), REG_WORD_LOW(REG_AREG(ea_reg)));
    } else if(size == INSTR_LONG) {
      instr_uop_push_reg_copy_byte(instr, source_reg, REG_AREG(ea_reg));
    }
  } else {
    ea_addr(instr, ea_mode, ea_reg, size, REG_VALUE(7));
    ea_write_to_addr(instr, size, REG_VALUE(7), source_reg);
  }
}

/* Special case since MOVE isn't equivalent to EA-READ+EA-WRITE 
 * TODO: reg,X and X,reg
 */
void ea_move(struct instr *instr,
             int src_ea_mode, int src_ea_reg,
             int dst_ea_mode, int dst_ea_reg,
             enum instr_sizes size) {
  ea_read(instr, src_ea_mode, src_ea_reg, size, REG_VALUE(0));
  switch(dst_ea_mode) {
  case EA_DN:
    /* Just copy value to Dn and return */
    if(size == INSTR_BYTE) {
      instr_uop_push_reg_copy_byte(instr, REG_WORD_LOW(REG_VALUE(0)), REG_WORD_LOW(REG_DREG(dst_ea_reg)));
    } else if(size == INSTR_WORD) {
      instr_uop_push_reg_copy_word(instr, REG_WORD_LOW(REG_VALUE(0)), REG_WORD_LOW(REG_DREG(dst_ea_reg)));
    } else if(size == INSTR_LONG) {
      instr_uop_push_reg_copy_long(instr, REG_VALUE(0), REG_DREG(dst_ea_reg));
    }
    return;
    break;
  case EA_AN:
    /* Unimplemented */
    break;
  case EA_MEM:
    ea_addr_mem(instr, dst_ea_reg, REG_VALUE(1));
    break;
  case EA_MEM_INC:
    ea_addr_mem_inc(instr, dst_ea_reg, size, REG_VALUE(1));
    break;
  case EA_MEM_DEC:
    ea_addr_mem_dec(instr, dst_ea_reg, size, REG_VALUE(1));
    break;
  case EA_MEM_OFFSET:
    ea_addr_mem_offset(instr, dst_ea_reg, REG_VALUE(1));
    break;
  case EA_MEM_OFFSET_REG:
    ea_addr_mem_offset_reg(instr, dst_ea_reg, size, REG_VALUE(1));
    break;
  case EA_EXTENDED:
    switch(dst_ea_reg) {
    case EA_PC_OFFSET:
    case EA_PC_OFFSET_REG:
      /* Not writable */
      break;
    case EA_SHORT:
      ea_addr_short(instr, REG_VALUE(1));
      break;
    case EA_LONG:
      ea_addr_long(instr, REG_VALUE(1));
      break;
    case EA_IMMEDIATE:
      /* Not writable */
      break;
    }
    break;
  }

  if(size == INSTR_LONG) {
    instr_uop_push_nop(instr);
    instr_uop_push_write_word(instr, REG_VALUE(1), REG_WORD_HIGH(REG_VALUE(0)));
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_nop(instr);
    instr_uop_push_write_next_word(instr, REG_VALUE(1), REG_WORD_LOW(REG_VALUE(0)));
  } else if(size == INSTR_WORD) {
    instr_uop_push_nop(instr);
    instr_uop_push_write_word(instr, REG_VALUE(1), REG_WORD_LOW(REG_VALUE(0)));
  } else if(size == INSTR_BYTE) {
    instr_uop_push_nop(instr);
    instr_uop_push_write_byte(instr, REG_VALUE(1), REG_WORD_LOW(REG_VALUE(0)));
  }
}
