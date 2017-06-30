#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* Bcc */
/* Flags unaffected */

#define SHORT_OP 0x6000
#define SHORT_OP_MASK 0xf000
#define LONG_OP 0x6000
#define LONG_OP_MASK 0xf0ff

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

/* If jump not taken, skip over one uOP (prefetch)
 * TODO: Only checking for BRA/EQ/NE right now...
 */
static void determine_short_jump(struct uop *uop, struct cpu *cpu) {
  int taken = 0;
  uint32_t jmp_distance;
  int z_flag;
  int condition;

  unused(uop);
  
  z_flag = (cpu->internal->r.sr&0x04)>>2;
  condition = (cpu->exec->op&0xf00)>>8;

  /* Zero flag set? TRUE => Jump on BEQ, FALSE => Jump on BNE */
  if(z_flag && (condition == 7)) {
    /* BEQ and Z set */
    taken = 1;
  }

  if(!z_flag && (condition == 6)) {
    /* BNE and Z clr */
    taken = 1;
  }

  /* BRA always jumps */
  if(condition == 0) {
    taken = 1;
  }
  
  if(taken) {
    /* Do not skip next uOP */
    jmp_distance = SIGN_EXT_BYTE(cpu->exec->op&0xff);
    cpu->internal->r.pc += jmp_distance - 2;
    cpu->exec->uops_pos++;
  } else {
    /* Skip two uOPs */
    cpu->exec->uops_pos += 3;
  }
}

/* For a long branch it's different.
 * Next uOP is skipped if jump is taken.
 * TODO: Only checking for BRA/EQ/NE right now...
 */
static void determine_long_jump(struct uop *uop, struct cpu *cpu) {
  int taken = 0;
  uint32_t jmp_distance;
  int z_flag;
  int condition;

  unused(uop);
  
  z_flag = (cpu->internal->r.sr&0x04)>>2;
  condition = (cpu->exec->op&0xf00)>>8;

  /* Zero flag set? TRUE => Jump on BEQ, FALSE => Jump on BNE */
  if(z_flag && (condition == 7)) {
    /* BEQ and Z set */
    taken = 1;
  }

  if(!z_flag && (condition == 6)) {
    /* BNE and Z clr */
    taken = 1;
  }

  /* BRA always jumps */
  if(condition == 0) {
    taken = 1;
  }
  
  if(taken) {
    /* At this point, before any other prefetches, the jump distance
     * is located in IRC
     */
    jmp_distance = SIGN_EXT_WORD(cpu->internal->r.irc);
    cpu->internal->r.pc += jmp_distance - 2;
    /* If jumping, skip next two uOPs */
    cpu->exec->uops_pos += 3;
  } else {
    /* Do not skip */
    cpu->exec->uops_pos++;
  }
}

static struct instr *instr_bcc_short_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "BCC.S");
  
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_short(instr, determine_short_jump, INSTR_UOP_SPECIAL);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  return instr;
}

static struct instr *instr_bcc_long_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "BCC");

  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_short(instr, determine_long_jump, INSTR_UOP_SPECIAL);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  
  instr_uop_push_end(instr);
  return instr;
}

void instr_bcc_setup(struct cpu *cpu) {
  struct instr *instr_short, *instr_long;
  instr_short = instr_bcc_short_setup(cpu);
  instr_long = instr_bcc_long_setup(cpu);
  
  cpu_instr_register(cpu, SHORT_OP, SHORT_OP_MASK, instr_short);
  cpu_instr_register(cpu, LONG_OP, LONG_OP_MASK, instr_long);
}
