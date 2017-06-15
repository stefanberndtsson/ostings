#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* Bcc */

#define SHORT_OP 0x6000
#define SHORT_OP_MASK 0xf000
#define LONG_OP 0x6000
#define LONG_OP_MASK 0xff00

/*
 * uOPs (Jump taken, short):
 * NOP 
 * NOP (determine_jump)
 * PREFETCH
 * NOP
 * PREFETCH
 *
 * uOPs (Jump not taken):
 * NOP 
 * NOP (determine_jump)
 * NOP
 * PREFETCH
 *
 */

/* If jump not taken, skip over one uOP (prefetch) */
static void determine_short_jump(struct uop *uop, struct cpu *cpu) {
  int taken = 1;
  uint32_t jmp_distance;

  if(taken) {
    /* Do not skip next uOP */
    jmp_distance = SIGN_EXT_BYTE(cpu->exec->op&0xff);
    cpu->internal->r.pc += jmp_distance - 2;
    cpu->exec->uops_pos++;
  } else {
    /* Skip one uOP */
    cpu->exec->uops_pos += 2;
  }
}

static struct instr *instr_bcc_short_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;

  instr_uop_push_nop(instr);
  instr_uop_push_short(instr, determine_short_jump, INSTR_UOP_SPECIAL);
  instr_uop_push_prefetch_into(instr, REG_VALUE_L_TO_REG_W(0));
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  return instr;
}

/* TODO: Unimplemented */
static struct instr *instr_bcc_long_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;

  instr_uop_push_end(instr);
  return instr;
}

struct instr *instr_bcc_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = instr_bcc_short_setup(cpu);
  
  cpu_instr_register(cpu, SHORT_OP, SHORT_OP_MASK, instr);
  
  return instr;
}

