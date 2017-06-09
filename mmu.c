#include "common.h"
#include "hw.h"
#include "mmu.h"

/* Register a memory segment for a particular hardware object 
 * This builds the memory map
 */
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
  mmu->hws = hws;

  return mmu;
}
