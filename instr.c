#include "instr.h"
#define UOP_ONLY 1
#include "instr/uop.h"
#undef UOP_ONLY

void instr_uop_push_full(struct instr *instr, instr_uop *uop, enum instr_uops code,
                         LONG data1, LONG data2, enum instr_sizes size, enum instr_extend ext) {
  struct uop *new_uop;

  new_uop = (struct uop *)ostis_alloc(sizeof(struct uop));
  new_uop->uop = uop;
  new_uop->data1 = data1;
  new_uop->data2 = data2;
  new_uop->size = size;
  new_uop->ext = ext;
  new_uop->code = code;
  instr->uops[instr->uops_count] = new_uop;
  instr->uops_count++;
  if(global_debug) {
    printf("Adding Code: %d, Data1: %d, Data2: %d, Size: %d\n", code, data1, data2, size);
  }
}

void instr_uop_push_short(struct instr *instr, instr_uop *uop, enum instr_uops code) {
  instr_uop_push_full(instr, uop, code, 0, 0, INSTR_LONG, EXT_NONE);
}

void instr_uop_push(struct instr *instr, enum instr_uops code,
                    LONG data1, LONG data2, enum instr_sizes size, enum instr_extend ext) {
  
  switch(code) {
  case INSTR_UOP_END:
    instr_uop_push_full(instr, uop_end, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_NOP:
    instr_uop_push_full(instr, uop_nop, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_NOPCNT:
    instr_uop_push_full(instr, uop_nopcnt, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_BOOT_PREFETCH:
    instr_uop_push_full(instr, uop_boot_prefetch, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_PREFETCH:
    instr_uop_push_full(instr, uop_prefetch, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_READ_BYTE:
    instr_uop_push_full(instr, uop_read_byte, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_READ_WORD:
    instr_uop_push_full(instr, uop_read_word, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_READ_NEXT_WORD:
    instr_uop_push_full(instr, uop_read_next_word, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_WRITE_BYTE:
    instr_uop_push_full(instr, uop_write_byte, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_WRITE_WORD:
    instr_uop_push_full(instr, uop_write_word, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_WRITE_NEXT_WORD:
    instr_uop_push_full(instr, uop_write_next_word, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_SPECIAL: /* Should never be called */
    instr_uop_push_full(instr, uop_special, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_EA_SPECIAL: /* Should never be called */
    instr_uop_push_full(instr, uop_ea_special, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_REG_COPY:
    instr_uop_push_full(instr, uop_reg_copy, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_REG_SWAP:
    instr_uop_push_full(instr, uop_reg_swap, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_DEC_REG:
    instr_uop_push_full(instr, uop_dec_reg, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_INC_REG:
    instr_uop_push_full(instr, uop_inc_reg, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_ADD:
    instr_uop_push_full(instr, uop_add, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_SIGN_EXT_WORD:
    instr_uop_push_full(instr, uop_sign_ext_word, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_SIGN_EXT_LONG:
    instr_uop_push_full(instr, uop_sign_ext_long, code, data1, data2, size, ext);
    break;
  case INSTR_UOP_MAX_COUNT:
    break;
  }
}

void instr_uop_push_end(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_END, 0, 0, INSTR_LONG, EXT_NONE);
}

void instr_uop_push_nop(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_NOP, 0, 0, INSTR_LONG, EXT_NONE);
}

void instr_uop_push_boot_prefetch(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_BOOT_PREFETCH, 0, 0, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_prefetch(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_PREFETCH, 0, 0, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_read_byte(struct instr *instr, LONG address_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_READ_BYTE, address_reg, target_reg, INSTR_BYTE, EXT_NONE);
}

void instr_uop_push_read_word(struct instr *instr, LONG address_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_READ_WORD, address_reg, target_reg, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_read_next_word(struct instr *instr, LONG address_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, address_reg, target_reg, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_write_byte(struct instr *instr, LONG address_reg, LONG source_reg) {
  instr_uop_push(instr, INSTR_UOP_WRITE_BYTE, address_reg, source_reg, INSTR_BYTE, EXT_NONE);
}

void instr_uop_push_write_word(struct instr *instr, LONG address_reg, LONG source_reg) {
  instr_uop_push(instr, INSTR_UOP_WRITE_WORD, address_reg, source_reg, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_write_next_word(struct instr *instr, LONG address_reg, LONG source_reg) {
  instr_uop_push(instr, INSTR_UOP_WRITE_NEXT_WORD, address_reg, source_reg, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_reg_copy_byte(struct instr *instr, LONG src_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_REG_COPY, src_reg, target_reg, INSTR_BYTE, EXT_NONE);
}

void instr_uop_push_reg_copy_word(struct instr *instr, LONG src_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_REG_COPY, src_reg, target_reg, INSTR_WORD, EXT_NONE);
}

void instr_uop_push_reg_copy_long(struct instr *instr, LONG src_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_REG_COPY, src_reg, target_reg, INSTR_LONG, EXT_NONE);
}

void instr_uop_push_dec_reg(struct instr *instr, LONG reg, enum instr_sizes size) {
  instr_uop_push(instr, INSTR_UOP_DEC_REG, reg, 0, size, EXT_NONE);
}

void instr_uop_push_inc_reg(struct instr *instr, LONG reg, enum instr_sizes size) {
  instr_uop_push(instr, INSTR_UOP_INC_REG, reg, 0, size, EXT_NONE);
}

void instr_uop_push_add_word_to_long(struct instr *instr, LONG src_reg, LONG target_reg) {
  instr_uop_push(instr, INSTR_UOP_ADD, src_reg, target_reg, INSTR_WORD, EXT_LONG);
}

void instr_uop_push_sign_ext_to_word(struct instr *instr, LONG reg, enum instr_sizes from_size) {
  instr_uop_push(instr, INSTR_UOP_SIGN_EXT_WORD, reg, 0, from_size, EXT_WORD);
}

void instr_uop_push_sign_ext_to_long(struct instr *instr, LONG reg, enum instr_sizes from_size) {
  instr_uop_push(instr, INSTR_UOP_SIGN_EXT_LONG, reg, 0, from_size, EXT_LONG);
}

