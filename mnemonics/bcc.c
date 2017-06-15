#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "mnemonics.h"
#include "mmu.h"
#include "ea.h"

#define SHORT_OP 0x6000
#define SHORT_OP_MASK 0xf000
#define LONG_OP 0x6000
#define LONG_OP_MASK 0xff00

static char *bcc_comparison_codes[16] = {
  "BRA", "BSR", "BHI", "BLS", "BCC", "BCS", "BNE", "BEQ",
  "BVC", "BVS", "BPL", "BMI", "BGE", "BLT", "BGT", "BLE"
};

static char *mnemonics_bcc_short(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  int code;
  LONG destination;
  uint32_t jmp_distance;

  op = mmu_peek_word(cpu->mmu, addr);
  code = (op&0xf00)>>8;
  jmp_distance = SIGN_EXT_BYTE(op&0xff);
  destination = addr+jmp_distance+2;
  
  mnemonic = ostis_alloc(17);
  snprintf(mnemonic, 17, "%s.S $%06X", bcc_comparison_codes[code], destination);

  return mnemonic;
}

static char *mnemonics_bcc_long(struct cpu *cpu, LONG addr) {
  WORD op;
  char *mnemonic;
  int code;
  LONG destination;
  uint32_t jmp_distance;

  op = mmu_peek_word(cpu->mmu, addr);
  code = (op&0xf00)>>8;
  jmp_distance = SIGN_EXT_BYTE(mmu_peek_word(cpu->mmu, addr+2));
  destination = addr+jmp_distance+2;
  
  mnemonic = ostis_alloc(15);
  snprintf(mnemonic, 15, "%s $%06X", bcc_comparison_codes[code], destination);

  return mnemonic;
}

void mnemonics_bcc_setup(struct cpu *cpu) {
  printf("DEBUG: Before: %ld\n", (uintptr_t)cpu->internal->mnemonics[0x660a]);
  mnemonics_register(cpu, SHORT_OP, SHORT_OP_MASK, mnemonics_bcc_short);
  printf("DEBUG: Middle: %ld\n", (uintptr_t)cpu->internal->mnemonics[0x660a]);
  mnemonics_register(cpu, LONG_OP, LONG_OP_MASK, mnemonics_bcc_long);
  printf("DEBUG: After: %ld\n", (uintptr_t)cpu->internal->mnemonics[0x660a]);
}
