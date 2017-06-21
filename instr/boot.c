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
  unused(uop);
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
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  /* Read Stack Pointer from BootROM */
  instr_uop_push_read_word(instr, REG_VALUE(0), REG_WORD_HIGH(REG_VALUE(1)));
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_read_next_word(instr, REG_VALUE(0), REG_WORD_LOW(REG_VALUE(1)));
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_VALUE(1), REG_AREG(7));
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_VALUE(1), REG_SSP);

  /* Read Program Counter from BootROM */
  instr_uop_push_nop(instr);
  instr_uop_push_read_word(instr, REG_VALUE(2), REG_WORD_HIGH(REG_VALUE(1)));
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_read_next_word(instr, REG_VALUE(2), REG_WORD_LOW(REG_VALUE(1)));
  instr_uop_push_nop(instr);
  instr_uop_push_reg_copy_long(instr, REG_VALUE(1), REG_PC);

  /* Do an extra IRC-Prefetch before a normal Prefetch */
  instr_uop_push_nop(instr);
  instr_uop_push_boot_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);
  return instr;
}

