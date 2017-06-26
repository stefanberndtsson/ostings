#include "common.h"
#include "hw.h"
#include "cpu.h"
#include "mmu.h"
#include "ram.h"
#include "rom.h"
#include "bootrom.h"
#include "cart.h"
#include "psg.h"

static struct hw **hw_initialize() {
  int i;
  struct hw **hws;
  struct hw *hw;
  hws = (struct hw **)ostis_alloc(sizeof(struct hw *) * HW_COUNT);

  for(i=0;i<HW_COUNT;i++) {
    hw = (struct hw *)ostis_alloc(sizeof(struct hw));
    hws[i] = hw;
    hws[i]->tick = (tick *)0;
  }
  
  return hws;
}

void hw_register(struct hw **hws, enum hw_components id, tick *tick, void *data) {
  hws[id]->id = id;
  hws[id]->tick = tick;
  hws[id]->data = data;
}

struct hw **hw_setup() {
  struct hw **hws;

  hws = hw_initialize();

  hw_register(hws, HW_CPU, cpu_tick, cpu_setup(hws));
  hw_register(hws, HW_MMU, mmu_tick, mmu_setup(hws));
  hw_register(hws, HW_RAM, NULL, ram_setup(hws));
  hw_register(hws, HW_ROM, NULL, rom_setup(hws));
  hw_register(hws, HW_BOOTROM, NULL, bootrom_setup(hws));
  hw_register(hws, HW_CART, NULL, cart_setup(hws));
  hw_register(hws, HW_PSG, NULL, psg_setup(hws));

  return hws;
}

