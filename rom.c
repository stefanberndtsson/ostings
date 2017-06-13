#include "common.h"
#include "mmu.h"
#include "rom.h"

#define ROMSTART 0x00fc0000
#define ROMSIZE 196608
#define ROMFILE "tos.img"

static void dummy_fill_rom(struct rom *rom) {
  FILE *fp;

  fp = fopen(ROMFILE, "rb");
  fread(rom->memory, 1, ROMSIZE, fp);
  fclose(fp);
}

static BYTE rom_read_byte(struct rom *rom, LONG addr) {
  return rom->memory[addr-ROMSTART];
}

static WORD rom_read_word(struct rom *rom, LONG addr) {
  return (rom_read_byte(rom, addr)<<8)|rom_read_byte(rom, addr+1);
}

static WORD rom_peek_word(struct rom *rom, LONG addr) {
  return rom_read_word(rom, addr);
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
  
  area = mmu_create_area((read_byte_t *)rom_read_byte,
                         (read_word_t *)rom_read_word,
                         NULL, NULL,
                         (read_word_t *)rom_peek_word,
                         rom, MMU_NOT_PROTECTED);
  mmu_register_area(hws[HW_MMU]->data, rom->start, rom->size, area);
  
  return rom;
}
