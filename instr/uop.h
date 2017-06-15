#ifndef OSTIS_UOPS_H
#define OSTIS_UOPS_H

#ifndef UOP_ONLY
#include "instr.h"
#endif

/*
 * uOPs have a function pointer, a code, and four parameters.
 * The four parameters are:
 *
 * DATA1:  Up to 32bit of uOP specific data
 * DATA2:  Another 32bits of data
 * SIZE:   BYTE/WORD/LONG size of operation, when applicable
 * EXT:    Sign extension directives (NONE/WORD/LONG)
 */

/* Do nothing for one period */
void uop_nop(struct uop *uop, struct cpu *);


/* Do nothing for DATA1 number of periods */
void uop_nopcnt(struct uop *, struct cpu *);


/* Prefetch WORD into IRC, but no other IR* modifications */
void uop_boot_prefetch(struct uop *, struct cpu *);


/* Move IRC to IR, Prefetch WORD into IRC, Move IR to IRD */
void uop_prefetch(struct uop *, struct cpu *);


/* Same as prefetch, except also store WORD in a register */
void uop_prefetch_into(struct uop *, struct cpu *);


/* PREFETCH_INTO and PREFETCH_NEXT_INTO is similar to READ_WORD and READ_NEXT_WORD
 * except for prefetch operations rather than read.
 */
void uop_prefetch_next_into(struct uop *, struct cpu *);

/* 
 * READ/WRITE
 *
 * All the READ and WRITE operations work with either BYTE or WORD sizes. There are
 * only 16 data pins, and therefor no LONG read/write available.
 * 
 * DATA1 is source (address for READ, data register for WRITE)
 * DATA2 is target (data register for READ, address for WRITE)
 * SIZE
 * 
 * For read, DATA1 is a register number corresponding to the LONG registers, 
 * used in full as an address (from the cpu->internal->r.l array)
 * For write, DATA2 serves the same purpose.
 *
 * For read, DATA2 is a register number in the WORD registers (cpu->internal->r.w)
 * and for write, DATA1 serves that purpose.
 * 
 * There are therefor some special cases regarding what address and reg number is
 * used, depending on what read/write uOP is used, documented beside each one below.
 *
 * The term "reg DATA1" means, take the number in DATA1, use it as an index in either
 * r.w or r.l, depending on situation. Same thing for DATA2.
 */


/* Read BYTE from address reg DATA1, 
 * write it to LOW BYTE of reg DATA2.
 * Sign extend if specified, if extending to LONG, sign extend into reg (DATA2 - 1).
 */
void uop_read_byte(struct uop *, struct cpu *);


/* Read WORD from address in reg DATA1,
 * write it to WORD of reg DATA2.
 * Sign extend if specified.
 */
void uop_read_word(struct uop *, struct cpu *);


/* Read WORD from (address in reg DATA1) + 2,
 * write it to WORD of reg (DATA2 + 1)
 */
void uop_read_next_word(struct uop *, struct cpu *);


/* Write BYTE from LOWest BYTE of reg DATA1,
 * into address of reg DATA2.
 */
void uop_write_byte(struct uop *, struct cpu *);


/* Write WORD from reg DATA1,
 * into address of reg DATA2.
 */
void uop_write_word(struct uop *, struct cpu *);


/* Write WORD from (reg DATA1 + 1),
 * into address of reg DATA2 + 2.
 */
void uop_write_next_word(struct uop *, struct cpu *);


/* Copy reg DATA1 to reg DATA2, sign extend if requested, copy only as large as size */
void uop_reg_copy(struct uop *, struct cpu *);


/* Swap the contents of reg DATA1 and reg DATA2, obey size */
void uop_reg_swap(struct uop *, struct cpu *);


/* This is a dummy function */
void uop_special(struct uop *, struct cpu *);


/* This is a dummy function */
void uop_ea_special(struct uop *, struct cpu *);


/* Subtract size appropriate value from reg DATA1 */
void uop_predec_reg(struct uop *, struct cpu *);


/* Add size appropriate value to reg DATA1 */
void uop_postinc_reg(struct uop *, struct cpu *);


/* This is a dummy function */
void uop_end(struct uop *, struct cpu *);

// void uop_spec(struct uop *, struct cpu *);
// void uop_unop(struct cpu *, LONG);
// void uop_unopcnt(struct cpu *, LONG);
// void uop_boot_prefetch(struct cpu *, LONG);
// void uop_prog_read(struct cpu *, LONG);
// void uop_read_byte(struct cpu *, LONG);
// void uop_read_word(struct cpu *, LONG);
// void uop_read_next_word(struct cpu *, LONG);
// void uop_write_byte(struct cpu *, LONG);
// void uop_write_word(struct cpu *, LONG);
// void uop_write_next_word(struct cpu *, LONG);
// void uop_data_to_value_low(struct cpu *, LONG);
// void uop_data_to_value_high(struct cpu *, LONG);
// void uop_value_low_to_data(struct cpu *, LONG);
// void uop_value_high_to_data(struct cpu *, LONG);
// void uop_ird_to_value_low(struct cpu *, LONG);
// void uop_ird_to_value_high(struct cpu *, LONG);
// void uop_value0_swap(struct cpu *, LONG);
// void uop_value0_to_dreg_byte(struct cpu *, LONG);
// void uop_value0_to_dreg_word(struct cpu *, LONG);
// void uop_value0_to_dreg_long(struct cpu *, LONG);
// void uop_value0_to_dreg_byte_sext(struct cpu *, LONG);
// void uop_value0_to_dreg_word_sext(struct cpu *, LONG);
// void uop_value0_to_areg_byte(struct cpu *, LONG);
// void uop_value0_to_areg_word(struct cpu *, LONG);
// void uop_value0_to_areg_long(struct cpu *, LONG);
// void uop_value0_to_areg_byte_sext(struct cpu *, LONG);
// void uop_value0_to_areg_word_sext(struct cpu *, LONG);
// void uop_dreg_to_value0_byte(struct cpu *, LONG);
// void uop_dreg_to_value0_word(struct cpu *, LONG);
// void uop_dreg_to_value0_long(struct cpu *, LONG);
// void uop_dreg_to_value0_byte_sext(struct cpu *, LONG);
// void uop_dreg_to_value0_word_sext(struct cpu *, LONG);
// void uop_areg_to_value0_byte(struct cpu *, LONG);
// void uop_areg_to_value0_word(struct cpu *, LONG);
// void uop_areg_to_value0_long(struct cpu *, LONG);
// void uop_areg_to_value0_byte_sext(struct cpu *, LONG);
// void uop_areg_to_value0_word_sext(struct cpu *, LONG);
// void uop_end(struct cpu *, LONG);

#endif /* OSTIS_UOPS_H */
