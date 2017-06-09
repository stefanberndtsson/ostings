#include <stdio.h>
#include "common.h"
#include "hw.h"
#include "mmu.h"
#include "cpu.h"

/* These functions fetch their address and read/write data via the cpu->external interface 
 * to somewhat simulate a CPU read/write request.
 * Read data will be written to cpu->external->data, and at some point, either immediately,
 * or after waitstates, cpu->external->data_available will be set, telling the CPU that
 * the data can be used by the instruction in question.
 * Writing works in a similar way.
 *
 * This all applies when the area is MMU protected. If not, everything is done immediately,
 * and data_available is set right away.
 */
void mmu_read_byte(struct mmu *mmu) {
  /* TODO: All */
}

void mmu_read_word(struct mmu *mmu) {
  LONG addr;
  struct mmu_area *area;
  
  addr = mmu->cpu->external->address;
  area = mmu->areas[addr];

  mmu->cpu->external->data = area->read_word(area->data, addr);
  
  if(area->mmu_protected == MMU_NOT_PROTECTED) {
    mmu->cpu->external->data_available = 1;
  } else {
    mmu->read_in_progress = 1;
  }
  
  printf("DEBUG: Read Word from $%08x\n", addr);
}

void mmu_write_byte(struct mmu *mmu) {
  /* TODO: All */
}

void mmu_write_word(struct mmu *mmu) {
  /* TODO: All */
}


/* Register a memory segment for a particular hardware object 
 * This builds the memory map
 */
void mmu_register_area(struct mmu *mmu, LONG start, LONG size, struct mmu_area *area) {
  LONG i;
  
  for(i=start;i<start+size;i++) {
    mmu->areas[i] = area;
  }
}

/* Helper function to create an area for registration 
 */
struct mmu_area *mmu_create_area(void *read_byte, void *read_word,
                                 void *write_byte, void *write_word,
                                 void *data, enum mmu_protection protected) {
  struct mmu_area *area;
  
  area = (struct mmu_area *)ostis_alloc(sizeof(struct mmu_area));
  area->read_byte = (read_byte_t *)read_byte;
  area->read_word = (read_word_t *)read_word;
  area->write_byte = (write_byte_t *)write_byte;
  area->write_word = (write_word_t *)write_word;
  area->data = data;
  area->mmu_protected = protected;

  return area;
}

struct mmu *mmu_setup(struct hw **hws) {
  struct mmu *mmu;

  mmu = (struct mmu *)ostis_alloc(sizeof(struct mmu));
  mmu->cpu = hws[HW_CPU]->data;
  mmu->cpu->mmu = mmu;
  mmu->hws = hws;
  mmu->read_in_progress = 0;
  mmu->write_in_progress = 0;

  return mmu;
}

void mmu_tick(struct hw *hw) {
  //  struct mmu *mmu;
  //  mmu = (struct mmu *)hw->data;
  printf("DEBUG: Ticking MMU...\n");
}
