#ifndef OSTIS_INSTR_H
#define OSTIS_INSTR_H

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
  INSTR_UOP_UNOP,
  INSTR_UOP_UNOPCNT,
  INSTR_UOP_PROG_READ,
  INSTR_UOP_READ_LOW,
  INSTR_UOP_READ_HIGH,
  INSTR_UOP_READ_WORD,
  INSTR_UOP_WRITE_LOW,
  INSTR_UOP_WRITE_HIGH,
  INSTR_UOP_WRITE_WORD,
  INSTR_UOP_EXECUTE,
  INSTR_UOP_BOOT_PREFETCH
};


/* struct instr:
 * Instruction specific data
 *
 * cpu:  Pointer to the CPU state
 * uops: Array of pointers to uop structs
 *
 */
struct instr {
  struct cpu *cpu;
  WORD op;
  enum instr_uops uops_types[MAX_UOPS];
  instr_uop *uops[MAX_UOPS];
};

struct instr *instr_boot_setup(WORD, struct cpu *);

#endif /* OSTIS_INSTR_H */
