#include <memory.h>
#include "cpu.h"
#include "instr.h"
#include "uop.h"

/* BOOT */

/*
 * uOPs:
 * UNOP
 * READ_WORD (SP-HIGH)
 * UNOP (handle word)
 * READ_WORD (SP-LOW)
 * UNOP (handle word)
 * READ_WORD (PC-HIGH)
 * UNOP (handle word)
 * READ_WORD (PC-LOW)
 * UNOP (handle word)
 * BOOT_PREFETCH
 * UNOP
 * PROG_READ
 */

static void handle_stack_pointer(struct cpu *cpu) {
  cpu->internal->a[7] <<= 16;
  cpu->internal->a[7] |= cpu->external->data;
  cpu->internal->ssp <<= 16;
  cpu->internal->ssp |= cpu->external->data;
}

static void handle_program_counter(struct cpu *cpu) {
  cpu->internal->pc <<= 16;
  cpu->internal->pc |= cpu->external->data;
}

struct instr *instr_boot_setup(WORD op, struct cpu *cpu) {
  struct instr *instr;
  instr_uop *uops[13] = {
    uop_unop,
    uop_read_word, handle_stack_pointer, uop_read_word, handle_stack_pointer,
    uop_read_word, handle_program_counter, uop_read_word, handle_program_counter,
    uop_boot_prefetch, uop_unop, uop_prog_read, uop_end
  };
  
  enum instr_uops types[13] = {
    INSTR_UOP_UNOP,
    INSTR_UOP_READ_WORD, INSTR_UOP_UNOP, INSTR_UOP_READ_WORD, INSTR_UOP_UNOP,
    INSTR_UOP_READ_WORD, INSTR_UOP_UNOP, INSTR_UOP_READ_WORD, INSTR_UOP_UNOP,
    INSTR_UOP_BOOT_PREFETCH, INSTR_UOP_UNOP, INSTR_UOP_PROG_READ, INSTR_UOP_END
  };

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr->op = op;
  memcpy(instr->uops, uops, sizeof(uops));
  memcpy(instr->uops_types, types, sizeof(types));
  return instr;
}

