#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* CMPI */
/* Flags:
 * X - Not affected
 * N - If (EA - value) is negative
 * Z - If EA same as value
 * V - If (EA - value) overflows
 * C - If (EA - value) carries
 */

#define OP 0x0C00
#define OP_MASK 0xff00
#define BUILD_OP(size, ea_mode, ea_reg) (0x0C00|(size<<6)|(ea_mode<<3)|ea_reg)
/* 0000 1100 1011 1001 */

/*
 * uOPs (fc0026):
 *
 * Get immediate:
 * 0 NOP
 * 1 Prefetch_into
 * 2 NOP
 * 3 Prefetch_next_into
 * 
 * Get EA-addr:
 * 4 NOP
 * 5 Prefetch_into
 * 6 NOP
 * 7 Prefetch_next_into
 *
 * Get EA-value:
 * 8 NOP
 * 9 Read_word
 * 10 NOP
 * 11 Read_next_word
 *
 * Compare:
 * 12 Special
 * 13 Prefetch
 * End
 *
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
  SET_NZVC(cpu, n, z, v, c);
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
  if(uop->size == INSTR_BYTE) {
    immediate &= 0xff;
    fetched &= 0xff;
    size_mask = 0x80;
  }
  if(uop->size == INSTR_WORD) {
    immediate &= 0xffff;
    fetched &= 0xffff;
    size_mask = 0x8000;
  }
  if(uop->size == INSTR_LONG) {
    size_mask = 0x80000000;
  }
  
  set_flags_cmp(cpu, immediate&size_mask, fetched&size_mask, result&size_mask, result);
  
  cpu->exec->uops_pos++;
}

static void add_ea_variant(struct cpu *cpu, int size, int ea_mode, int ea_reg) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "CMPI");

  ea_read_immediate(instr, REG_VALUE(0), size);
  ea_read(instr, ea_mode, ea_reg, size, REG_VALUE(2));
  instr_uop_push_nop(instr);
  instr_uop_push_full(instr, compare, INSTR_UOP_SPECIAL, REG_VALUE(1), REG_VALUE(2), size, EXT_NONE);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, BUILD_OP(size, ea_mode, ea_reg), 0xffff, instr);
}

void instr_cmpi_setup(struct cpu *cpu) {
  int size,ea_mode,ea_reg;

  for(ea_mode=0;ea_mode<8;ea_mode++) {
    for(ea_reg=0;ea_reg<8;ea_reg++) {
      for(size=0;size<3;size++) {
        if(BUILD_OP(size, ea_mode, ea_reg) == 0x0cad) {
          global_debug = 1;
        } else {
          global_debug = 0;
        }
        add_ea_variant(cpu, size, ea_mode, ea_reg);
      }
    }
  }
}

