#ifndef OSTIS_INSTR_H
#define OSTIS_INSTR_H

/* enum instr_sizes:
 * Instruction sizes
 * UNSPECIFIED: There probably is a size, but when used with this enum, it's not relevant
 * BYTE:        Well, BYTE size :)
 * WORD:        Get it already?
 * LONG:        Really? LONG? Yay, finally!
 */
enum instr_sizes {
  INSTR_BYTE=0,
  INSTR_WORD=1,
  INSTR_LONG=2,
};

#include "cpu.h"

#define MAX_UOPS 512

struct instr;
typedef void instr_uop(struct cpu *, LONG);

/* enum instr_micro_ops:
 * Micro operations available for an instruction
 *
 * END:           End of UOP chain
 * UNOP:          Do nothing
 * UNOPCNT:       Do nothing multiple times, with counter in queue
 * PROG_READ:     Read one WORD from program RAM (extension words or instruction prefetch)
 * READ_LOW:      Read Low BYTE from Address into Data
 * READ_HIGH:     Read High BYTE from Address into Data
 * READ_WORD:     Read WORD from Address into Data
 * WRITE_LOW:     Write Low BYTE from Data to Address
 * WRITE_HIGH:    Write High BYTE from Data to Address
 * WRITE_WORD:    Write WORD from Data to Address
 * EXECUTE:       Run instruction specific function from queue 
 * BOOT_PREFETCH: Special PROG_READ only into IRC, always followed by a PROG_READ
 */
enum instr_uops {
  INSTR_UOP_END=0,
  INSTR_UOP_UNOP,
  INSTR_UOP_UNOPCNT,
  INSTR_UOP_PROG_READ,
  INSTR_UOP_READ_BYTE,
  INSTR_UOP_READ_WORD,
  INSTR_UOP_READ_NEXT_WORD,
  INSTR_UOP_WRITE_BYTE,
  INSTR_UOP_WRITE_WORD,
  INSTR_UOP_WRITE_NEXT_WORD,
  INSTR_UOP_SPEC,
  INSTR_UOP_SPEC_EA,
  INSTR_UOP_BOOT_PREFETCH,
  INSTR_UOP_DATA_TO_VALUE_LOW,
  INSTR_UOP_DATA_TO_VALUE_HIGH,
  INSTR_UOP_VALUE_LOW_TO_DATA,
  INSTR_UOP_VALUE_HIGH_TO_DATA,
  INSTR_UOP_IRD_TO_VALUE_LOW,
  INSTR_UOP_IRD_TO_VALUE_HIGH,
  INSTR_UOP_VALUE0_SWAP,
  INSTR_UOP_VALUE0_TO_DREG_BYTE,
  INSTR_UOP_VALUE0_TO_DREG_WORD,
  INSTR_UOP_VALUE0_TO_DREG_LONG,
  INSTR_UOP_VALUE0_TO_DREG_BYTE_SEXT,
  INSTR_UOP_VALUE0_TO_DREG_WORD_SEXT,
  INSTR_UOP_VALUE0_TO_AREG_BYTE,
  INSTR_UOP_VALUE0_TO_AREG_WORD,
  INSTR_UOP_VALUE0_TO_AREG_LONG,
  INSTR_UOP_VALUE0_TO_AREG_BYTE_SEXT,
  INSTR_UOP_VALUE0_TO_AREG_WORD_SEXT,
  INSTR_UOP_DREG_TO_VALUE0_BYTE,
  INSTR_UOP_DREG_TO_VALUE0_WORD,
  INSTR_UOP_DREG_TO_VALUE0_LONG,
  INSTR_UOP_DREG_TO_VALUE0_BYTE_SEXT,
  INSTR_UOP_DREG_TO_VALUE0_WORD_SEXT,
  INSTR_UOP_AREG_TO_VALUE0_BYTE,
  INSTR_UOP_AREG_TO_VALUE0_WORD,
  INSTR_UOP_AREG_TO_VALUE0_LONG,
  INSTR_UOP_AREG_TO_VALUE0_BYTE_SEXT,
  INSTR_UOP_AREG_TO_VALUE0_WORD_SEXT,
  INSTR_UOP_MAX_COUNT
};

/* struct uop:
 * Micro operation and its data
 *
 * uop:  Function for the micro-op
 * data: Each micro-op can have one data value, like register number or value-field
 * code: Code for the micro operation
 */
struct uop {
  instr_uop *uop;
  LONG data;
  enum instr_uops code;
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
  int uops_count;
};

void instr_uop_push_full(struct instr *, instr_uop *, enum instr_uops, LONG);
void instr_uop_push(struct instr *, enum instr_uops, LONG);
void instr_uop_push_unop(struct instr *);
void instr_uop_push_prog_read(struct instr *);
void instr_uop_push_end(struct instr *);
struct instr *instr_boot_setup(struct cpu *);
struct instr *instr_unimplemented_setup(struct cpu *);
struct instr *instr_nop_setup(struct cpu *);
struct instr *instr_reset_setup(struct cpu *);
struct instr *instr_move_to_sr_setup(struct cpu *);
struct instr *instr_cmpi_setup(struct cpu *);

#endif /* OSTIS_INSTR_H */
