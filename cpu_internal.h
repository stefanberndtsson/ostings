#ifndef OSTIS_CPU_INTERNAL_H
#define OSTIS_CPU_INTERNAL_H

#include "common.h"
#include "instr.h"

typedef char *mnemonics_t(struct cpu *, LONG addr);

/* enum cpu_main_states:
 * States the CPU can be in
 *
 * RESET:   After RESET instruction or BOOT, do initialisation sequence
 * HALT:    CPU is halted, do nothing until released
 * RUNNING: Normal operation
 */

enum cpu_main_states {
  CPU_RESET,
  CPU_HALT,
  CPU_RUNNING
};

/* struct cpu_internal:
 * Bits internal to the CPU
 * 
 * Registers:
 * A0-7: Address registers
 * D0-7: Data registers
 * USP:  Usermode Stack Pointer
 * SSP:  Supervisor Stack Pointer
 * PC:   Program Counter
 * SR:   Status Register
 * IRC:  Instruction Register Capture (Prefetch Stage 1)
 * IR:   Instruction Register (Prefetch Stage 2)
 * IRD:  Instruction Register Decoder (Prefetch Stage 3)
 *
 * Other stored values:
 * Cycles:     Total amount of CPU cycles executed so far
 * ICycle:     Cycles used by the last executed instruction
 * Main state: Overall state of CPU (RESET, HALT, RUNNING)
 * 
 * Instr:      All possible op-codes, mapped to structs
 */

struct cpu_internal {
  /* Registers */
  LONG a[8];
  LONG d[8];
  LONG usp;
  LONG ssp;
  LONG pc;
  WORD sr;
  WORD irc;
  WORD ir;
  WORD ird;

  /* Other values */
  uint64_t cycles;
  uint32_t icycle;
  enum cpu_main_states main_state;

  struct instr *instr[65536];
  mnemonics_t *mnemonics[65536];
};

#endif /* OSTIS_CPU_INTERNAL_H */
