#ifndef OSTIS_INSTR_H
#define OSTIS_INSTR_H

/* enum instr_sizes:
 * Instruction sizes
 * 
 * BYTE:        Well, BYTE size :)
 * WORD:        Get it already?
 * LONG:        Really? LONG? Yay, finally!
 */
enum instr_sizes {
  INSTR_BYTE=0,
  INSTR_WORD=1,
  INSTR_LONG=2,
};

/* enum instr_extend:
 * Data sign extension. Specifies to what size data should be sign extended.
 * Paired with an "enum instr_sizes" to tell what size to extend from.
 * 
 * NONE:        No extending.
 * WORD:        Sign extend to WORD size
 * LONG:        Sign extend to LONG size
 */
enum instr_extend {
  EXT_NONE=0,
  EXT_WORD=1,
  EXT_LONG=2,
};

#include "cpu.h"

#define MAX_UOPS 512

struct instr;
struct uop;
typedef void instr_uop(struct uop *, struct cpu *);

/* enum instr_micro_ops:
 * Micro operations available for an instruction
 *
 * 
 */
enum instr_uops {
  INSTR_UOP_END=0,
  INSTR_UOP_NOP=1,
  INSTR_UOP_NOPCNT,
  INSTR_UOP_BOOT_PREFETCH,
  INSTR_UOP_PREFETCH,
  INSTR_UOP_READ_BYTE,
  INSTR_UOP_READ_WORD,
  INSTR_UOP_READ_NEXT_WORD,
  INSTR_UOP_WRITE_BYTE,
  INSTR_UOP_WRITE_WORD,
  INSTR_UOP_WRITE_NEXT_WORD,
  INSTR_UOP_REG_COPY,
  INSTR_UOP_REG_SWAP,
  INSTR_UOP_SPECIAL,
  INSTR_UOP_EA_SPECIAL,
  INSTR_UOP_DEC_REG,
  INSTR_UOP_INC_REG,
  INSTR_UOP_ADD,
  INSTR_UOP_SIGN_EXT_WORD,
  INSTR_UOP_SIGN_EXT_LONG,
  INSTR_UOP_MAX_COUNT
};

/* struct uop:
 * Micro operation and its data
 *
 * uop:   Function for the micro-op
 * code:  Code for the micro operation
 * data1: Data value specific to the uOP in question
 * data2: Second data value
 * size:  BYTE/WORD/LONG size of operation, when relevant
 * ext:   Some uOPs can sign extend values, this specifies if, and to what size
 */
struct uop {
  instr_uop *uop;
  enum instr_uops code;
  LONG data1;
  LONG data2;
  enum instr_sizes size;
  enum instr_extend ext;
};

/* struct instr:
 * Instruction specific data
 *
 * cpu:        Pointer to the CPU state
 * uops:       Array of uop functions
 *
 */
struct instr {
  struct cpu *cpu;
  struct uop *uops[MAX_UOPS];
  char code[32];
  int uops_count;
};

void instr_uop_push_full(struct instr *, instr_uop *, enum instr_uops, LONG, LONG, enum instr_sizes, enum instr_extend);
void instr_uop_push_short(struct instr *, instr_uop *, enum instr_uops);
void instr_uop_push(struct instr *, enum instr_uops, LONG, LONG, enum instr_sizes, enum instr_extend);
void instr_uop_push_nop(struct instr *);
void instr_uop_push_boot_prefetch(struct instr *);
void instr_uop_push_prefetch(struct instr *);
void instr_uop_push_end(struct instr *);
void instr_uop_push_read_byte(struct instr *, LONG, LONG);
void instr_uop_push_read_word(struct instr *, LONG, LONG);
void instr_uop_push_read_next_word(struct instr *, LONG, LONG);
void instr_uop_push_write_byte(struct instr *, LONG, LONG);
void instr_uop_push_write_word(struct instr *, LONG, LONG);
void instr_uop_push_write_next_word(struct instr *, LONG, LONG);
void instr_uop_push_reg_copy_byte(struct instr *, LONG, LONG);
void instr_uop_push_reg_copy_word(struct instr *, LONG, LONG);
void instr_uop_push_reg_copy_long(struct instr *, LONG, LONG);
void instr_uop_push_dec_reg(struct instr *, LONG, enum instr_sizes);
void instr_uop_push_inc_reg(struct instr *, LONG, enum instr_sizes);
void instr_uop_push_add_word_to_long(struct instr *, LONG, LONG);
void instr_uop_push_sign_ext_to_word(struct instr *, LONG, enum instr_sizes);
void instr_uop_push_sign_ext_to_long(struct instr *, LONG, enum instr_sizes);

/* Very special boot instruction */
struct instr *instr_boot_setup(struct cpu *);

/* Instruction definitions */
void instr_unimplemented_setup(struct cpu *);
void instr_nop_setup(struct cpu *);
void instr_reset_setup(struct cpu *);
void instr_move_to_sr_setup(struct cpu *);
void instr_cmpi_setup(struct cpu *);
void instr_bcc_setup(struct cpu *);
void instr_lea_setup(struct cpu *);
void instr_suba_setup(struct cpu *);
void instr_jmp_setup(struct cpu *);
void instr_move_b_setup(struct cpu *);
void instr_btst_setup(struct cpu *);
void instr_moveq_setup(struct cpu *);

#endif /* OSTIS_INSTR_H */
