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
typedef void instr_uop(struct cpu *);

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
  INSTR_UOP_UNOP=1,
  INSTR_UOP_UNOPCNT=2,
  INSTR_UOP_PROG_READ=3,
  INSTR_UOP_READ_LOW=4,
  INSTR_UOP_READ_HIGH=5,
  INSTR_UOP_READ_WORD=6,
  INSTR_UOP_WRITE_LOW=7,
  INSTR_UOP_WRITE_HIGH=8,
  INSTR_UOP_WRITE_WORD=9,
  INSTR_UOP_EXECUTE=10,
  INSTR_UOP_BOOT_PREFETCH=11,
  INSTR_UOP_MAX_COUNT
};

/* struct instr:
 * Instruction specific data
 *
 * cpu:        Pointer to the CPU state
 * uops_types: Type-codes for uOPs
 * uops:       Array of uop functions
 * mnemonic:   Function for returning mnemonic string of instruction
 * size:       Size of instruction (BYTE/WORD/LONG)
 *
 */
struct instr {
  struct cpu *cpu;
  enum instr_uops uops_types[MAX_UOPS];
  instr_uop *uops[MAX_UOPS];
  LONG uop_data[MAX_UOPS];
};

struct instr *instr_boot_setup(struct cpu *);
struct instr *instr_unimplemented_setup(struct cpu *);
struct instr *instr_nop_setup(struct cpu *);
struct instr *instr_reset_setup(struct cpu *);
struct instr *instr_move_to_sr_setup(struct cpu *);

#endif /* OSTIS_INSTR_H */
