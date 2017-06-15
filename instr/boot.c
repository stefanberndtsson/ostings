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

static void set_addresses(struct uop *uop, struct cpu *cpu) {
  cpu->internal->r.value[0] = 0x00000000;
  cpu->internal->r.value[2] = 0x00000004;
  cpu->internal->main_state = CPU_RUNNING;
  cpu->exec->uops_pos++;
}

struct instr *instr_boot_setup(struct cpu *cpu) {
  struct instr *instr;
  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  instr_uop_push_short(instr, set_addresses, INSTR_UOP_SPECIAL);
  /* Read Stack Pointer from BootROM */
  instr_uop_push(instr, INSTR_UOP_READ_WORD, REG_VALUE_TO_REG_L(0), REG_VALUE_H_TO_REG_W(1), INSTR_WORD, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, REG_VALUE_TO_REG_L(0), REG_VALUE_H_TO_REG_W(1), INSTR_WORD, EXT_NONE);
  instr_uop_push(instr, INSTR_UOP_REG_COPY, REG_VALUE_TO_REG_L(1), REG_AREG_TO_REG_L(7), INSTR_LONG, EXT_NONE);
  instr_uop_push(instr, INSTR_UOP_REG_COPY, REG_VALUE_TO_REG_L(1), REG_SSP_TO_REG_L, INSTR_LONG, EXT_NONE);

  /* Read Program Counter from BootROM */
  instr_uop_push(instr, INSTR_UOP_READ_WORD, REG_VALUE_TO_REG_L(2), REG_VALUE_H_TO_REG_W(1), INSTR_WORD, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push(instr, INSTR_UOP_READ_NEXT_WORD, REG_VALUE_TO_REG_L(2), REG_VALUE_H_TO_REG_W(1), INSTR_WORD, EXT_NONE);
  instr_uop_push(instr, INSTR_UOP_REG_COPY, REG_VALUE_TO_REG_L(1), REG_PC_TO_REG_L, INSTR_LONG, EXT_NONE);

  /* Do an extra IRC-Prefetch before a normal Prefetch */
  instr_uop_push_boot_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  return instr;
}

