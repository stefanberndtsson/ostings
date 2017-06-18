#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* CMPI */

#define OP 0x0C00
#define OP_MASK 0xff00

/*
 * uOPs (LONG):
 * UNOP (Assemble left value)
 * PROG_READ
 * UNOP (Assemble left value)
 * PROG_READ
 * UNOP (Assemble address)
 * PROG_READ (EA)
 * UNOP (Assemble address)
 * PROG_READ (EA)
 * UNOP
 * READ_WORD
 * UNOP (Assemble right value)
 * READ_WORD
 * UNOP (Assemble right value)
 * PROG_READ
 */

static void set_flags_cmp(struct cpu *cpu, int src_masked, int dst_masked, int result_masked, int result) {
  int n,z,v,c;
  n = z = v = c = 0;
  
  /* Negative? */
  if(result_masked) {
    n = 1;
  }

  /* Zero? */
  if(result==0) {
    z = 1;
  }

  /* oVerflow? */
  if((!src_masked && dst_masked && !result_masked) || (src_masked && !dst_masked && result_masked)) {
    v = 1;
  }
  
  /* Carry? */
  if((src_masked && !dst_masked) || (result_masked && !dst_masked) || (src_masked && result_masked)) {
    c = 1;
  }
  cpu->internal->r.sr = (cpu->internal->r.sr & 0xff00) | (n<<3) | (z<<2) | (v<<1) | (c<<0);
}

/* Compare what's in VALUE[0] (immediate) with VALUE[2] (EA-fetched)
 */
static void compare(struct uop *uop, struct cpu *cpu) {
  LONG immediate, fetched;
  LONG size_mask;
  LONG result;

  immediate = cpu->internal->r.value[0];
  fetched = cpu->internal->r.value[2];

  result = fetched - immediate;
  if(uop->size == INSTR_BYTE) { size_mask = 0x80; }
  if(uop->size == INSTR_WORD) { size_mask = 0x8000; }
  if(uop->size == INSTR_LONG) { size_mask = 0x80000000; }
  
  set_flags_cmp(cpu, immediate&size_mask, fetched&size_mask, result&size_mask, result);
  
  cpu->exec->uops_pos++;
}

void instr_cmpi_setup(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;

  /* Fetch LONG immediate value */
  ea_read_immediate(instr, REG_VALUE_H_TO_REG_W(0), INSTR_LONG);

  /* Fetch EA */
  ea_read_abs_long(instr, 1, REG_VALUE_H_TO_REG_W(2), INSTR_LONG);

  /* Do comparison between VALUE[0] and VALUE[2] */
  instr_uop_push_full(instr, compare, INSTR_UOP_SPECIAL, REG_VALUE_TO_REG_L(1), REG_VALUE_TO_REG_L(2), INSTR_LONG, EXT_NONE);

  /* End with prefetch */
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, OP, OP_MASK, instr);
}

