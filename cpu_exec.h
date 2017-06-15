#ifndef OSTIS_CPU_EXEC_H
#define OSTIS_CPU_EXEC_H

#include "common.h"
#include "cpu.h"
#include "instr.h"

#define MAX_UOPS_PARAMS 512

/* enum exec_states:
 * States available for an instruction
 *
 * START:   Instruction decoded, but not yet consumed any cycles
 * EXECUTE: uOPs are being executed
 * END:     Instruction has consumed its last cycle
 */
enum exec_states {
  EXEC_START=0,
  EXEC_EXECUTE,
  EXEC_END
};

#define MAX_VALUES 10
/* struct cpu_exec:
 * Current instruction being executed
 *
 * OP:            Opcode of the currently active instruction
 * Instr:         The instruction object of the executed instruction
 * State:         State of the current instruction
 * Data In:       Input data from EA or similar
 * Data Out:      Input data from EA or similar
 * Size:          Size of operation (BYTE/WORD/LONG)
 * uOPs Data Pos: Position within the uOPs Data
 * uOPs Data:     Data used by uOPs
 * Cycles:        Cycles consumed so far by the instruction
 * Flag:          Various flaggning depending on current uOP
 *
 */
struct cpu_exec {
  WORD op;
  struct instr *instr;
  mnemonics_t *mnemonic;
  enum exec_states state;
  int uops_pos;
  LONG instr_addr;
  uint32_t cycles;
  int counter;
};

#endif /* OSTIS_CPU_EXEC_H */
