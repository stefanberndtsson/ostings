#include "common.h"
#include "hw.h"
#include "mmu.h"

void mmu_register_area(struct mmu *mmu, LONG start, LONG size, struct mmu_area *area) {
  LONG i;
  LONG start_bank,end_bank;
  start_bank = start >> 8;
  end_bank = (start+size) >> 8;
  if(end_bank > 0xffff) {
    FATAL("END BANK outside memory area");
  }
  for(i=start_bank;i<end_bank;i++) {
    mmu->areas[i] = area;
  }
}

struct mmu *mmu_setup(struct hw **hws) {
  struct mmu *mmu;

  mmu = (struct mmu *)ostis_alloc(sizeof(struct mmu));
  mmu->cpu = hws[HW_CPU]->data;
  mmu->hws = hws;

  return mmu;
}
