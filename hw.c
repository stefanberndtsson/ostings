#include "common.h"
#include "hw.h"
#include "cpu.h"

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

struct hw **hw_setup() {
  struct hw **hws;

  hws = hw_initialize();

  hws[HW_CPU]->id = HW_CPU;
  hws[HW_CPU]->tick = cpu_tick;
  hws[HW_CPU]->data = cpu_setup();

  return hws;
}

