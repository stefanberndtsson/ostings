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
 * Registers (r.*):
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
 * r.w:  Array of WORDs overlapping above registers
 * r.l:  Array of LONGs overlapping above registers
 *
 * Other stored values:
 * Cycles:     Total amount of CPU cycles executed so far
 * ICycle:     Cycles used by the last executed instruction
 * Main state: Overall state of CPU (RESET, HALT, RUNNING)
 * 
 * Instr:      All possible op-codes, mapped to structs
 */

#define LITTLE_ENDIAN 1

struct cpu_internal {
  /* Registers */
  union {
    struct registers {
      LONG a[8];       // 0 (LONG offset from start)
      LONG d[8];       // 8
      LONG usp;        // 16
      LONG ssp;        // 17
      LONG pc;         // 18
      LONG value[8];   // 19
      WORD padding1;   // 27
      WORD sr;         // 
      WORD padding2;   // 28
      WORD irc;        // 
      WORD padding3;   // 29
      WORD ir;         // 
      WORD padding4;   // 30
      WORD ird;        //
    } r;
    WORD w[31*2];
    LONG l[31];
  };

  /* Other values */
  uint64_t cycles;
  uint32_t icycle;
  enum cpu_main_states main_state;

  struct instr *instr[65536];
  mnemonics_t *mnemonics[65536];
};


#ifdef LITTLE_ENDIAN

/* REG_xxx is the offset in the LONG array where the LONG version of the
 * register is located.
 * Therefor 2*REG_xxx is the LOW WORD (LITTLE ENDIAN) in the WORD array
 * and 2*REG_xxx+1 is the HIGH WORD (LITTLE ENDIAN) in the WORD array
 * For BIG ENDIAN, this is inverted.
 *
 * All registers are treated as LONG. If they are actually WORD, there is
 * a padding WORD to make room for the full LONG.
 */
#define REG_AREG_OFFSET 0
#define REG_AREG(x) (x+REG_AREG_OFFSET)

#define REG_DREG_OFFSET (REG_AREG_OFFSET+8)
#define REG_DREG(x) (x+REG_DREG_OFFSET)

#define REG_USP_OFFSET (REG_DREG_OFFSET+8)
#define REG_USP (REG_USP_OFFSET)

#define REG_SSP_OFFSET (REG_USP_OFFSET+1)
#define REG_SSP (REG_SSP_OFFSET)

#define REG_PC_OFFSET (REG_SSP_OFFSET+1)
#define REG_PC (REG_PC_OFFSET)

#define REG_VALUE_OFFSET (REG_PC_OFFSET+1)
#define REG_VALUE(x) (x+REG_VALUE_OFFSET)

#define REG_SR_OFFSET (REG_VALUE_OFFSET+8)
#define REG_SR (REG_SR_OFFSET)

#define REG_IRC_OFFSET (REG_SR_OFFSET+1)
#define REG_IRC (REG_IRC_OFFSET)

#define REG_IR_OFFSET (REG_IRC_OFFSET+1)
#define REG_IR (REG_IR_OFFSET)

#define REG_IRD_OFFSET (REG_IR_OFFSET+1)
#define REG_IRD (REG_IRD_OFFSET)

#define REG_WORD_HIGH(long_reg) (long_reg*2+1)
#define REG_WORD_LOW(long_reg) (long_reg*2+0)

#define REG_SR_W  REG_WORD_HIGH(REG_SR)
#define REG_IRC_W REG_WORD_HIGH(REG_IRC)
#define REG_IR_W  REG_WORD_HIGH(REG_IR)
#define REG_IRD_W REG_WORD_HIGH(REG_IRD)

/* TODO: Properly handle BIG_ENDIAN */
#endif


#endif /* OSTIS_CPU_INTERNAL_H */
