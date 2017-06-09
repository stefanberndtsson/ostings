#include "common.h"
#include "mmu.h"
#include "ram.h"

#define RAMSTART 0x00000008
#define RAMSIZE 4*1048576-8

static void dummy_fill_ram(struct ram *ram) {
  int i;
  for(i=0;i<10000;i++) {
    ram->memory[i*2+0] = 0x4e;
    ram->memory[i*2+1] = 0x71;
  }
}

static BYTE ram_read_byte(struct ram *ram, LONG address) {
  return ram->memory[address-RAMSTART];
}

static WORD ram_read_word(struct ram *ram, LONG address) {
  return (ram_read_byte(ram, address)<<16)|ram_read_byte(ram, address+1);
}

static void ram_write_byte(struct ram *ram, LONG address, BYTE value) {
  ram->memory[address-RAMSTART] = value;
}

static void ram_write_word(struct ram *ram, LONG address, WORD value) {
  ram_write_byte(ram, address, value>>8);
  ram_write_byte(ram, address, value&0xff);
}

struct ram *ram_setup(struct hw **hws) {
  struct ram *ram;
  struct mmu_area *area;

  ram = (struct ram *)ostis_alloc(sizeof(struct ram));
  ram->hws = hws;
  ram->memory = (BYTE *)ostis_alloc(RAMSIZE);
  ram->start = RAMSTART;
  ram->size = RAMSIZE;
  ram->end = RAMSTART+RAMSIZE-1;

  dummy_fill_ram(ram);

  area = (struct mmu_area *)ostis_alloc(sizeof(struct mmu_area));
  area->read_byte = (read_byte_t *)ram_read_byte;
  area->read_word = (read_word_t *)ram_read_word;
  area->write_byte = (write_byte_t *)ram_write_byte;
  area->write_word = (write_word_t *)ram_write_word;
  area->data = ram;
  
  mmu_register_area(hws[HW_MMU]->data, ram->start, ram->size, area);
  
  return ram;
}
