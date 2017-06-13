#ifndef OSTIS_UOPS_H
#define OSTIS_UOPS_H

#ifndef UOP_ONLY
#include "instr.h"
#endif

void uop_spec(struct cpu *, LONG);
void uop_unop(struct cpu *, LONG);
void uop_unopcnt(struct cpu *, LONG);
void uop_boot_prefetch(struct cpu *, LONG);
void uop_prog_read(struct cpu *, LONG);
void uop_read_byte(struct cpu *, LONG);
void uop_read_word(struct cpu *, LONG);
void uop_read_next_word(struct cpu *, LONG);
void uop_write_byte(struct cpu *, LONG);
void uop_write_word(struct cpu *, LONG);
void uop_write_next_word(struct cpu *, LONG);
void uop_data_to_value_low(struct cpu *, LONG);
void uop_data_to_value_high(struct cpu *, LONG);
void uop_value_low_to_data(struct cpu *, LONG);
void uop_value_high_to_data(struct cpu *, LONG);
void uop_ird_to_value_low(struct cpu *, LONG);
void uop_ird_to_value_high(struct cpu *, LONG);
void uop_value0_swap(struct cpu *, LONG);
void uop_value0_to_reg_byte(struct cpu *, LONG);
void uop_value0_to_reg_word(struct cpu *, LONG);
void uop_value0_to_reg_long(struct cpu *, LONG);
void uop_value0_to_reg_byte_sext(struct cpu *, LONG);
void uop_value0_to_reg_word_sext(struct cpu *, LONG);
void uop_reg_to_value0_byte(struct cpu *, LONG);
void uop_reg_to_value0_word(struct cpu *, LONG);
void uop_reg_to_value0_long(struct cpu *, LONG);
void uop_reg_to_value0_byte_sext(struct cpu *, LONG);
void uop_reg_to_value0_word_sext(struct cpu *, LONG);
void uop_end(struct cpu *, LONG);

#endif /* OSTIS_UOPS_H */
