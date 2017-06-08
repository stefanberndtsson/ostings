#ifndef OSTIS_CPU_INTERNAL_H
#define OSTIS_CPU_INTERNAL_H

#include "common.h"

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
 * Cycles:        Total amount of CPU cycles executed so far
 * ICycle:        Cycles used by the last executed instruction
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
};

#endif /* OSTIS_CPU_INTERNAL_H */
