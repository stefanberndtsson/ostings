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

static void unimplemented_halt(struct uop *uop, struct cpu *cpu) {
  unused(uop);
  printf("\n\n\n\nUnimplemented OP: %04X\n", cpu->exec->op);
  cpu_debug_info(cpu);
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0020));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0024));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0026));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0030));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc0032));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfc003c));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfa0004));
  printf("DEBUG: %s\n", mnemonics_at(cpu, 0xfa0006));
  exit(-97);
}

void instr_unimplemented_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_short(instr, unimplemented_halt, INSTR_UOP_SPECIAL);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, OP, OP_MASK, instr);
}

