#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define OP 0x1000
#define OP_MASK 0xf000

static char *mnemonics_move_b(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  char *src_ea_string,*dst_ea_string;
  int src_ea_mode, src_ea_reg, src_ea_offset;
  int dst_ea_mode, dst_ea_reg, dst_ea_offset;
  enum instr_sizes size;
  int alloc_size;

  op = mmu_peek_word(cpu->mmu, addr);
  size = INSTR_BYTE;

  src_ea_offset = 0;
  src_ea_mode = EA_MODE(op);
  src_ea_reg = EA_REG(op);
  src_ea_string = mnemonics_ea(cpu, addr, size, src_ea_mode, src_ea_reg, src_ea_offset);
  
  dst_ea_offset = 0;
  dst_ea_mode = EA_HIGH_MODE(op);
  dst_ea_reg = EA_HIGH_REG(op);
  dst_ea_string = mnemonics_ea(cpu, addr, size, dst_ea_mode, dst_ea_reg, dst_ea_offset);

  alloc_size = strlen(src_ea_string) + strlen(dst_ea_string) + 10;
  
  mnemonic = ostis_alloc(alloc_size);
  snprintf(mnemonic, alloc_size, "MOVE.B %s,%s", src_ea_string, dst_ea_string);

  return mnemonic;
}

void mnemonics_move_b_setup(struct cpu *cpu) {
  mnemonics_register(cpu, OP, OP_MASK, mnemonics_move_b);
}
