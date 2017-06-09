#include "common.h"
#include "mmu.h"
#include "bootrom.h"

#define BOOTROMSTART 0x00000000
#define BOOTROMSIZE 8

static void dummy_fill_bootrom(struct bootrom *bootrom) {
  bootrom->memory[0] = 0x60;
  bootrom->memory[1] = 0x2e;
  bootrom->memory[2] = 0x01;
  bootrom->memory[3] = 0x02;
  bootrom->memory[4] = 0x00;
  bootrom->memory[5] = 0xfc;
  bootrom->memory[6] = 0x00;
  bootrom->memory[7] = 0x30;
}

static BYTE bootrom_read_byte(struct bootrom *bootrom, LONG address) {
  return bootrom->memory[address-BOOTROMSTART];
}

static WORD bootrom_read_word(struct bootrom *bootrom, LONG address) {
  return (bootrom_read_byte(bootrom, address)<<8)|bootrom_read_byte(bootrom, address+1);
}

static void bootrom_write_byte(struct bootrom *bootrom, LONG address, BYTE value) {
  bootrom->memory[address-BOOTROMSTART] = value;
}

static void bootrom_write_word(struct bootrom *bootrom, LONG address, WORD value) {
  bootrom_write_byte(bootrom, address, value>>8);
  bootrom_write_byte(bootrom, address, value&0xff);
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
  
  area = mmu_create_area(bootrom_read_byte, bootrom_read_word,
                         bootrom_write_byte, bootrom_write_word,
                         bootrom, MMU_NOT_PROTECTED);
  mmu_register_area(hws[HW_MMU]->data, bootrom->start, bootrom->size, area);
  
  return bootrom;
}
