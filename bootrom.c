#include "common.h"
#include "mmu.h"
#include "bootrom.h"

#define BOOTROMSTART 0x00000000
#define BOOTROMSIZE 8
#define BOOTROMFILE "tos.img"

static void dummy_fill_bootrom(struct bootrom *bootrom) {
  FILE *fp;

  fp = fopen(BOOTROMFILE, "rb");
  fread(bootrom->memory, 1, BOOTROMSIZE, fp);
  fclose(fp);
}

static BYTE bootrom_read_byte(struct bootrom *bootrom, LONG addr) {
  return bootrom->memory[addr-BOOTROMSTART];
}

static WORD bootrom_read_word(struct bootrom *bootrom, LONG addr) {
  return (bootrom_read_byte(bootrom, addr)<<8)|bootrom_read_byte(bootrom, addr+1);
}

static WORD bootrom_peek_word(struct bootrom *bootrom, LONG addr) {
  return bootrom_read_word(bootrom, addr);
}

struct bootrom *bootrom_setup(struct hw **hws) {
  struct bootrom *bootrom;
  struct mmu_area *area;

  bootrom = (struct bootrom *)ostis_alloc(sizeof(struct bootrom));
  bootrom->hws = hws;
  bootrom->memory = (BYTE *)ostis_alloc(BOOTROMSIZE);
  bootrom->start = BOOTROMSTART;
  bootrom->size = BOOTROMSIZE;
  bootrom->end = BOOTROMSTART+BOOTROMSIZE-1;

  dummy_fill_bootrom(bootrom);
  
  area = mmu_create_area((read_byte_t *)bootrom_read_byte,
                         (read_word_t *)bootrom_read_word,
                         NULL, NULL,
                         (read_word_t *)bootrom_peek_word,
                         bootrom, MMU_NOT_PROTECTED, 0);
  mmu_register_area(hws[HW_MMU]->data, bootrom->start, bootrom->size, area);
  
  return bootrom;
}
