#include "common.h"
#include "mmu.h"
#include "rom.h"

#define ROMSTART 0x00fc0000
#define ROMSIZE 196608

static void dummy_fill_rom(struct rom *rom) {
  int i;
  for(i=0;i<10000;i++) {
    rom->memory[i*2+0] = 0x4e;
    rom->memory[i*2+1] = 0x71;
  }
}

static BYTE rom_read_byte(struct rom *rom, LONG address) {
  return rom->memory[address-ROMSTART];
}

static WORD rom_read_word(struct rom *rom, LONG address) {
  return (rom_read_byte(rom, address)<<16)|rom_read_byte(rom, address+1);
}

static void rom_write_byte(struct rom *rom, LONG address, BYTE value) {
  rom->memory[address-ROMSTART] = value;
}

static void rom_write_word(struct rom *rom, LONG address, WORD value) {
  rom_write_byte(rom, address, value>>8);
  rom_write_byte(rom, address, value&0xff);
}

struct rom *rom_setup(struct hw **hws) {
  struct rom *rom;
  struct mmu_area *area;

  rom = (struct rom *)ostis_alloc(sizeof(struct rom));
  rom->hws = hws;
  rom->memory = (BYTE *)ostis_alloc(ROMSIZE);
  rom->start = ROMSTART;
  rom->size = ROMSIZE;
  rom->end = ROMSTART+ROMSIZE-1;

  dummy_fill_rom(rom);
  
  area = (struct mmu_area *)ostis_alloc(sizeof(struct mmu_area));
  area->read_byte = (read_byte_t *)rom_read_byte;
  area->read_word = (read_word_t *)rom_read_word;
  area->write_byte = (write_byte_t *)rom_write_byte;
  area->write_word = (write_word_t *)rom_write_word;
  area->data = rom;
  
  mmu_register_area(hws[HW_MMU]->data, rom->start, rom->size, area);
  
  return rom;
}
