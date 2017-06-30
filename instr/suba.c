#include <stdio.h>
#include <memory.h>
#include "cpu.h"
#include "uop.h"
#include "ea.h"

/* SUBA */
/* Flags unaffected */

#define OP_REGS 0x90c0
#define OP_MASK_REGS 0xf0f0
#define BUILD_OP(size, ea_mode, ea_reg) (0x90c0|(size<<8)|(ea_mode<<3)|ea_reg)

/*
 * uOPs (Regs):
 * NOP
 * Prefetch
 * NOP (subtract)
 * NOP
 * 
 * uOPs (EA):
 * EA-uOPS
 * NOP
 * Prefetch
 * NOP (subtract)
 * NOP (all WORD and immediate-EA for LONG)
 */

/* Compare what's in VALUE[0] (immediate) with VALUE[2] (EA-fetched)
 */
static void subtract(struct uop *uop, struct cpu *cpu) {
  WORD op;
  LONG src_value;
  LONG result;
  int size_long;

  unused(uop);

  op = cpu->exec->op;
  size_long = op&0x100;
  
  if(EA_MODE(op) == EA_AN) { /* SRC: Address register */
    src_value = cpu->internal->r.a[EA_REG(op)];
  } else if(EA_MODE(op) == EA_DN) { /* SRC: Data register */
    src_value = cpu->internal->r.a[EA_REG(op)];
  } else {
    src_value = cpu->internal->r.value[0];
  }

  /* If WORD, sign extend to LONG before subtract */
  if(!size_long) {
    src_value = SIGN_EXT_WORD(src_value);
  }
  
  result = cpu->internal->r.a[EA_HIGH_REG(op)] - src_value;
  cpu->internal->r.a[EA_HIGH_REG(op)] = result;

  /* Special cases. WORD size take longer. So skip next two uOPs if: 
   * size == LONG && EA is NOT Immediate
   * otherwise, inc as usual.
   */
  if(size_long && ((EA_MODE(op) != EA_EXTENDED) ||
     ((EA_MODE(op) == EA_EXTENDED) && (EA_REG(op) != EA_IMMEDIATE)))) {
    cpu->exec->uops_pos += 3;
  } else {
    cpu->exec->uops_pos++;
  }
}

void instr_suba_setup_regs(struct cpu *cpu) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "SUBA");

  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_short(instr, subtract, INSTR_UOP_SPECIAL);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, OP_REGS, OP_MASK_REGS, instr);
}

void add_ea_variant(struct cpu *cpu, int size, int ea_mode, int ea_reg) {
  struct instr *instr;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;
  snprintf(instr->code, 31, "SUBA");

  ea_read(instr, ea_mode, ea_reg, size, REG_VALUE(0));
  
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_prefetch(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_short(instr, subtract, INSTR_UOP_SPECIAL);
  instr_uop_push_nop(instr);
  instr_uop_push_nop(instr);
  instr_uop_push_end(instr);

  cpu_instr_register(cpu, BUILD_OP(size, ea_mode, ea_reg), 0xf1ff, instr);
}

void instr_suba_setup_ea(struct cpu *cpu) {
  struct instr *instr;
  int size,ea_mode,ea_reg;

  instr = (struct instr *)ostis_alloc(sizeof(struct instr));
  instr->cpu = cpu;

  for(ea_mode=0;ea_mode<8;ea_mode++) {
    for(ea_reg=0;ea_reg<8;ea_reg++) {
      for(size=0;size<2;size++) {
        add_ea_variant(cpu, size, ea_mode, ea_reg);
      }
    }
  }
}

void instr_suba_setup(struct cpu *cpu) {
  /* Setup all EA variants first */
  instr_suba_setup_ea(cpu);

  /* Register variants must come after EA variants due to overlap issues otherwise */
  instr_suba_setup_regs(cpu);
}

