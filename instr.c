#include "instr.h"
#define UOP_ONLY 1
#include "instr/uop.h"
#undef UOP_ONLY

void instr_uop_push_full(struct instr *instr, instr_uop *uop, enum instr_uops code, LONG data) {
  struct uop *new_uop;

  new_uop = (struct uop *)ostis_alloc(sizeof(struct uop));
  new_uop->uop = uop;
  new_uop->data = data;
  new_uop->code = code;
  instr->uops[instr->uops_count] = new_uop;
  instr->uops_count++;
}

void instr_uop_push(struct instr *instr, enum instr_uops code, LONG data) {
  switch(code) {
  case INSTR_UOP_END:
    instr_uop_push_full(instr, uop_end, code, data);
    break;
  case INSTR_UOP_UNOP:
    instr_uop_push_full(instr, uop_unop, code, data);
    break;
  case INSTR_UOP_UNOPCNT:
    instr_uop_push_full(instr, uop_unopcnt, code, data);
    break;
  case INSTR_UOP_PROG_READ:
    instr_uop_push_full(instr, uop_prog_read, code, data);
    break;
  case INSTR_UOP_READ_BYTE:
    instr_uop_push_full(instr, uop_read_byte, code, data);
    break;
  case INSTR_UOP_READ_WORD:
    instr_uop_push_full(instr, uop_read_word, code, data);
    break;
  case INSTR_UOP_READ_NEXT_WORD:
    instr_uop_push_full(instr, uop_read_next_word, code, data);
    break;
  case INSTR_UOP_WRITE_BYTE:
    instr_uop_push_full(instr, uop_write_byte, code, data);
    break;
  case INSTR_UOP_WRITE_WORD:
    instr_uop_push_full(instr, uop_write_word, code, data);
    break;
  case INSTR_UOP_WRITE_NEXT_WORD:
    instr_uop_push_full(instr, uop_write_next_word, code, data);
    break;
  case INSTR_UOP_SPEC: /* Should never be called */
    instr_uop_push_full(instr, uop_spec, code, data);
    break;
  case INSTR_UOP_SPEC_EA: /* Should never be called */
    instr_uop_push_full(instr, uop_spec, code, data);
    break;
  case INSTR_UOP_BOOT_PREFETCH:
    instr_uop_push_full(instr, uop_boot_prefetch, code, data);
    break;
  case INSTR_UOP_DATA_TO_VALUE_LOW:
    instr_uop_push_full(instr, uop_data_to_value_low, code, data);
    break;
  case INSTR_UOP_DATA_TO_VALUE_HIGH:
    instr_uop_push_full(instr, uop_data_to_value_high, code, data);
    break;
  case INSTR_UOP_VALUE_LOW_TO_DATA:
    instr_uop_push_full(instr, uop_value_low_to_data, code, data);
    break;
  case INSTR_UOP_VALUE_HIGH_TO_DATA:
    instr_uop_push_full(instr, uop_value_high_to_data, code, data);
    break;
  case INSTR_UOP_IRD_TO_VALUE_LOW:
    instr_uop_push_full(instr, uop_ird_to_value_low, code, data);
    break;
  case INSTR_UOP_IRD_TO_VALUE_HIGH:
    instr_uop_push_full(instr, uop_ird_to_value_high, code, data);
    break;
  case INSTR_UOP_VALUE0_SWAP:
    instr_uop_push_full(instr, uop_value0_swap, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_DREG_BYTE:
    instr_uop_push_full(instr, uop_value0_to_dreg_byte, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_DREG_WORD:
    instr_uop_push_full(instr, uop_value0_to_dreg_word, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_DREG_LONG:
    instr_uop_push_full(instr, uop_value0_to_dreg_long, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_DREG_BYTE_SEXT:
    instr_uop_push_full(instr, uop_value0_to_dreg_byte_sext, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_DREG_WORD_SEXT:
    instr_uop_push_full(instr, uop_value0_to_dreg_word_sext, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_AREG_BYTE:
    instr_uop_push_full(instr, uop_value0_to_areg_byte, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_AREG_WORD:
    instr_uop_push_full(instr, uop_value0_to_areg_word, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_AREG_LONG:
    instr_uop_push_full(instr, uop_value0_to_areg_long, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_AREG_BYTE_SEXT:
    instr_uop_push_full(instr, uop_value0_to_areg_byte_sext, code, data);
    break;
  case INSTR_UOP_VALUE0_TO_AREG_WORD_SEXT:
    instr_uop_push_full(instr, uop_value0_to_areg_word_sext, code, data);
    break;
  case INSTR_UOP_DREG_TO_VALUE0_BYTE:
    instr_uop_push_full(instr, uop_dreg_to_value0_byte, code, data);
    break;
  case INSTR_UOP_DREG_TO_VALUE0_WORD:
    instr_uop_push_full(instr, uop_dreg_to_value0_word, code, data);
    break;
  case INSTR_UOP_DREG_TO_VALUE0_LONG:
    instr_uop_push_full(instr, uop_dreg_to_value0_long, code, data);
    break;
  case INSTR_UOP_DREG_TO_VALUE0_BYTE_SEXT:
    instr_uop_push_full(instr, uop_dreg_to_value0_byte_sext, code, data);
    break;
  case INSTR_UOP_DREG_TO_VALUE0_WORD_SEXT:
    instr_uop_push_full(instr, uop_dreg_to_value0_word_sext, code, data);
    break;
  case INSTR_UOP_AREG_TO_VALUE0_BYTE:
    instr_uop_push_full(instr, uop_areg_to_value0_byte, code, data);
    break;
  case INSTR_UOP_AREG_TO_VALUE0_WORD:
    instr_uop_push_full(instr, uop_areg_to_value0_word, code, data);
    break;
  case INSTR_UOP_AREG_TO_VALUE0_LONG:
    instr_uop_push_full(instr, uop_areg_to_value0_long, code, data);
    break;
  case INSTR_UOP_AREG_TO_VALUE0_BYTE_SEXT:
    instr_uop_push_full(instr, uop_areg_to_value0_byte_sext, code, data);
    break;
  case INSTR_UOP_AREG_TO_VALUE0_WORD_SEXT:
    instr_uop_push_full(instr, uop_areg_to_value0_word_sext, code, data);
    break;
  case INSTR_UOP_MAX_COUNT:
    break;
  }
}

void instr_uop_push_end(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_END, 0);
}

void instr_uop_push_unop(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_UNOP, 0);
}

void instr_uop_push_prog_read(struct instr *instr) {
  instr_uop_push(instr, INSTR_UOP_PROG_READ, 0);
}

