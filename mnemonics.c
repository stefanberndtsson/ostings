#include "cpu.h"
#include "mmu.h"
#include "mnemonics.h"

char *mnemonics_at(struct cpu *cpu, LONG addr) {
  WORD op;
  op = mmu_peek_word(cpu->mmu, addr);
  return cpu->internal->mnemonics[op](cpu, addr);
}

void mnemonics_setup(struct cpu *cpu) {
  mnemonics_unimplemented_setup(cpu);
  mnemonics_boot_setup(cpu);
  mnemonics_reset_setup(cpu);
  mnemonics_nop_setup(cpu);
  mnemonics_move_to_sr_setup(cpu);
  mnemonics_cmpi_setup(cpu);
  mnemonics_bcc_setup(cpu);
}

void mnemonics_register(struct cpu *cpu, WORD op, WORD op_mask, mnemonics_t *mnemonic) {
  int i;
  for(i=0;i<65536;i++) {
    if((i&op_mask) == (op&op_mask)) {
      cpu->internal->mnemonics[i] = mnemonic;
    }
  }
}

