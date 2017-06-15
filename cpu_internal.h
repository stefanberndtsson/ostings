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

struct cpu_internal {
  /* Registers */
  union {
    struct registers {
      LONG a[8];       // 0    0
      LONG d[8];       // 8   16
      LONG usp;        // 16  32
      LONG ssp;        // 17  34
      LONG pc;         // 18  36
      LONG value[8];   // 19  38  
      WORD sr;         //  -  54
      WORD irc;        //     55
      WORD ir;         //     56
      WORD ird;        //     57
    } r;
    WORD w[2*(8+8+1+1+1+8)+1+1+1+1];
    LONG l[8+8+1+1+1+8+2];
  };

  /* Other values */
  uint64_t cycles;
  uint32_t icycle;
  enum cpu_main_states main_state;

  struct instr *instr[65536];
  mnemonics_t *mnemonics[65536];
};

#define LITTLE_ENDIAN 1

#ifdef LITTLE_ENDIAN
/* TODO: Properly handle BIG_ENDIAN */
#define REG_AREG_OFFSET 0
#define REG_AREG_H_TO_REG_W(reg_num) ((REG_AREG_OFFSET*2)+(reg_num*2+1))
#define REG_AREG_L_TO_REG_W(reg_num) ((REG_AREG_OFFSET*2)+(reg_num*2))
#define REG_AREG_TO_REG_L(reg_num) ((REG_AREG_OFFSET)+(reg_num))

#define REG_DREG_OFFSET (REG_AREG_OFFSET+8)
#define REG_DREG_H_TO_REG_W(reg_num) ((REG_DREG_OFFSET*2)+(reg_num*2+1))
#define REG_DREG_L_TO_REG_W(reg_num) ((REG_DREG_OFFSET*2)+(reg_num*2))
#define REG_DREG_TO_REG_L(reg_num) ((REG_DREG_OFFSET)+(reg_num))

#define REG_USP_OFFSET (REG_DREG_OFFSET+8)
#define REG_USP_H_TO_REG_W ((REG_USP_OFFSET*2)+1)
#define REG_USP_L_TO_REG_W ((REG_USP_OFFSET*2))
#define REG_USP_TO_REG_L (REG_USP_OFFSET)

#define REG_SSP_OFFSET (REG_USP_OFFSET+1)
#define REG_SSP_H_TO_REG_W ((REG_SSP_OFFSET*2)+1)
#define REG_SSP_L_TO_REG_W ((REG_SSP_OFFSET*2))
#define REG_SSP_TO_REG_L (REG_SSP_OFFSET)

#define REG_PC_OFFSET (REG_SSP_OFFSET+1)
#define REG_PC_H_TO_REG_W ((REG_PC_OFFSET*2)+1)
#define REG_PC_L_TO_REG_W ((REG_PC_OFFSET*2))
#define REG_PC_TO_REG_L (REG_PC_OFFSET)

#define REG_VALUE_OFFSET (REG_PC_OFFSET+1)
#define REG_VALUE_H_TO_REG_W(reg_num) ((REG_VALUE_OFFSET*2)+(reg_num*2+1))
#define REG_VALUE_L_TO_REG_W(reg_num) ((REG_VALUE_OFFSET*2)+(reg_num*2))
#define REG_VALUE_TO_REG_L(reg_num) ((REG_VALUE_OFFSET)+(reg_num))

#define REG_SR_OFFSET (((REG_VALUE_OFFSET+8)*2)) /* All before are LONG */
#define REG_SR_TO_REG_W (REG_SR_OFFSET)

#define REG_IRC_OFFSET (REG_SR_OFFSET+1)
#define REG_IRC_TO_REG_W (REG_IRC_OFFSET)

#define REG_IR_OFFSET (REG_IRC_OFFSET+1)
#define REG_IR_TO_REG_W (REG_IR_OFFSET)

#define REG_IRD_OFFSET (REG_IR_OFFSET+1)
#define REG_IRD_TO_REG_W (REG_IRD_OFFSET)

#endif


#endif /* OSTIS_CPU_INTERNAL_H */
