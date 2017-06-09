#include <memory.h>
#include "cpu.h"
#include "instr.h"
#include "uop.h"

/* BOOT */

/*
 * uOPs:
 * UNOP (setup_address)
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
  cpu->external->address += 2;
  cpu->exec->uops_pos++;
}

static void handle_program_counter(struct cpu *cpu) {
  cpu->internal->pc <<= 16;
  cpu->internal->pc |= cpu->external->data;
  cpu->external->address += 2;
  cpu->exec->uops_pos++;
}

static void set_stack_pointer_address(struct cpu *cpu) {
  cpu->external->address = 0x00000000;
  cpu->internal->main_state = CPU_RUNNING;
  cpu->exec->uops_pos++;
}

struct instr *instr_boot_setup(struct cpu *cpu) {
  struct instr *instr;
  instr_uop *uops[13] = {
    set_stack_pointer_address,
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
  memcpy(instr->uops, uops, sizeof(uops));
  memcpy(instr->uops_types, types, sizeof(types));
  return instr;
}

