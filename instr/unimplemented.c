#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "mnemonics.h"
#include "uop.h"

/* UNIMPLEMENTED */

#define OP 0x0000
#define OP_MASK 0x0000

/*
 * uOPs:
 * Unimplemented instruction, report and exit.
 *
 */

static void unimplemented_halt(struct cpu *cpu) {
  printf("\n\n\n\nUnimplemented OP: %04X\n", cpu->exec->op);
  cpu_debug_info(cpu);
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0020));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0024));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0026));
  exit(-97);
}

struct instr *instr_unimplemented_setup(struct cpu *cpu) {
  struct instr *instr;
  instr_uop *uops[1] = {
    unimplemented_halt
  };
  enum instr_uops types[3] = {
    INSTR_UOP_UNOP
  };

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  memcpy(instr->uops, uops, sizeof(uops));
  memcpy(instr->uops_types, types, sizeof(types));

  cpu_instr_register(cpu, OP, OP_MASK, instr);
  
  return instr;
}

