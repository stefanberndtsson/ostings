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

static void set_stack_pointer(struct cpu *cpu, LONG value_num) {
  cpu->internal->r.a[7] = cpu->exec->value[value_num];
  cpu->internal->r.ssp = cpu->internal->r.a[7];
  cpu->exec->uops_pos++;
}

static void set_program_counter(struct cpu *cpu, LONG value_num) {
  cpu->internal->r.pc = cpu->exec->value[value_num];
  cpu->exec->uops_pos++;
}

static void set_addresses(struct cpu *cpu, LONG data) {
  cpu->exec->value[0] = 0x00000000;
  cpu->exec->value[2] = 0x00000004;
  cpu->internal->main_state = CPU_RUNNING;
  cpu->exec->uops_pos++;
}

struct instr *instr_boot_setup(struct cpu *cpu) {
  struct instr *instr;
  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_full(instr, set_addresses, INSTR_UOP_SPEC, 0);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 0);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_HIGH, 1);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, 0);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_LOW, 1);
  instr_uop_push(instr, INSTR_UOP_READ_WORD, 2);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_HIGH, 0);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, 2);
  instr_uop_push(instr, INSTR_UOP_DATA_TO_VALUE_LOW, 0);
  instr_uop_push_full(instr, set_stack_pointer, INSTR_UOP_SPEC, 1);
  instr_uop_push_full(instr, set_program_counter, INSTR_UOP_SPEC, 0);
  instr_uop_push(instr, INSTR_UOP_BOOT_PREFETCH, 0);
  instr_uop_push_unop(instr);
  instr_uop_push_prog_read(instr);
  instr_uop_push_end(instr);
  return instr;
}

