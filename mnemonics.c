#include "cpu.h"
#include "mnemonics.h"

void mnemonics_setup(struct cpu *cpu) {
  mnemonics_unimplemented_setup(cpu);
  mnemonics_boot_setup(cpu);
  mnemonics_reset_setup(cpu);
  mnemonics_nop_setup(cpu);
  mnemonics_move_to_sr_setup(cpu);
  mnemonics_cmpi_setup(cpu);
}

void mnemonics_register(struct cpu *cpu, WORD op, WORD op_mask, mnemonics_t *mnemonic) {
  int i;
  for(i=0;i<65536;i++) {
    if((i&op_mask) == (op&op_mask)) {
      cpu->internal->mnemonics[i] = mnemonic;
    }
  }
}

