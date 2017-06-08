#ifndef OSTIS_CPU_INSTR_H
#define OSTIS_CPU_INSTR_H

#include "common.h"

/* enum instr_states:
 * States available for an instruction
 *
 * START:          Instruction decoded, but not yet consumed any cycles
 * UNOP:           Micro-NOP. Do nothing for this pass
 * PROG_READ:      Read one WORD from program RAM (extension words or instruction prefetch)
 * MEM_READ_LOW:   Read low BYTE from data RAM
 * MEM_READ_HIGH:  Read high BYTE from data RAM
 * MEM_READ_WORD:  Read WORD from data RAM
 * MEM_WRITE_LOW:  Write low BYTE to data RAM
 * MEM_WRITE_HIGH: Write high BYTE to data RAM
 * MEM_WRITE_WORD: Write WORD to data RAM
 * EXEC:           Instruction specific operation state
 * END:            Instruction has consumed its last cycle
 * RESET:          State after RESET or BOOT
 */
enum instr_states {
  INSTR_START=0,
  INSTR_UNOP,
  INSTR_PROG_READ,
  INSTR_MEM_READ_LOW,
  INSTR_MEM_READ_HIGH,
  INSTR_MEM_READ_WORD,
  INSTR_MEM_WRITE_LOW,
  INSTR_MEM_WRITE_HIGH,
  INSTR_MEM_WRITE_WORD,
  INSTR_EXEC,
  INSTR_END,
  INSTR_RESET
};

/* struct cpu_instr:
 * Instruction related values and state
 *
 * OP:      Opcode of the currently active instruction
 * State:   State of the current instruction
 * Counter: Counter used by some states
 * Cycles:  Cycles consumed so far by the instruction
 *
 * Each state is repeated as many times as counter states, except START and END
 */
struct cpu_instr {
  WORD op;
  enum instr_states state;
  int counter;
  uint32_t cycles;
};

#endif /* OSTIS_CPU_INSTR_H */
