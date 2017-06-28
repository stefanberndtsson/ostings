#include <stdio.h>
#include "common.h"
#include "hw.h"
#include "mmu.h"
#include "cpu.h"

/* Check if MMU is on a tick aligned with when CPU is allowed access to
 * MMU protected memory
 */
static int mmu_in_cpu_window(struct mmu *mmu) {
  if(((mmu->tick_alignment-1) % 8) < 4) {
    return 0;
  } else {
    return 1;
  }
}

/* These functions fetch their address and read/write data via the cpu->external interface 
 * to somewhat simulate a CPU read/write request.
 * Read data will be written to cpu->external->data, and at some point, either immediately,
 * or after waitstates, cpu->external->data_available will be set, telling the CPU that
 * the data can be used by the instruction in question.
 * Writing works in a similar way.
 *
 * This all applies when the area is MMU protected. If not, everything is done immediately,
 * and data_available is set right away.
 *
 * TODO: extra waitstates for specific memory types.
 */
void mmu_read_byte(struct mmu *mmu) {
  LONG addr;
  struct mmu_area *area;
  
  addr = mmu->cpu->external->address;
  area = mmu->areas[addr];

  printf("DEBUG-MMU: READ_BYTE(%08X)\n", addr);
  
  mmu->cpu->external->data = area->read_byte(area->data, addr);
  
  /* Initialise waitstate counter depending on area setting */
  if(!mmu->read_in_progress) {
    mmu->waitstate_counter = area->waitstates;
  }

  /* Allow immediate read when:
   * Waitstate counter has reached 0
   * ...AND...
   * Area is not protected, or mmu is in CPU access window.
   *
   * Otherwise set read in progress flag.
   */
  if(mmu->waitstate_counter <= 0 && (area->mmu_protected == MMU_NOT_PROTECTED || mmu_in_cpu_window(mmu))) {
    mmu->cpu->external->data_available = 1;
  } else {
    mmu->read_in_progress = 1;
  }
  
  printf("DEBUG: Read Byte from $%08x\n", addr);
}

void mmu_read_word(struct mmu *mmu) {
  LONG addr;
  struct mmu_area *area;
  
  addr = mmu->cpu->external->address;
  area = mmu->areas[addr];

  printf("DEBUG-MMU: READ_WORD(%08X)\n", addr);
  
  mmu->cpu->external->data = area->read_word(area->data, addr);
  
  /* Initialise waitstate counter depending on area setting */
  if(!mmu->read_in_progress) {
    mmu->waitstate_counter = area->waitstates;
  }

  /* Allow immediate read when:
   * Waitstate counter has reached 0
   * ...AND...
   * Area is not protected, or mmu is in CPU access window.
   *
   * Otherwise set read in progress flag.
   */
  if(mmu->waitstate_counter <= 0 && (area->mmu_protected == MMU_NOT_PROTECTED || mmu_in_cpu_window(mmu))) {
    mmu->cpu->external->data_available = 1;
  } else {
    mmu->read_in_progress = 1;
  }
  
  printf("DEBUG: Read Word from $%08x\n", addr);
}

void mmu_write_byte(struct mmu *mmu) {
  LONG addr;
  BYTE data;
  struct mmu_area *area;
  
  addr = mmu->cpu->external->address;
  data = mmu->cpu->external->data&0xff;
  area = mmu->areas[addr];

  printf("DEBUG-MMU: WRITE_BYTE(%08X, %02X)\n", addr, data);

  /* Initialise waitstate counter depending on area setting */
  if(!mmu->write_in_progress) {
    mmu->waitstate_counter = area->waitstates;
  }

  /* Allow immediate write when:
   * Waitstate counter has reached 0
   * ...AND...
   * Area is not protected, or mmu is in CPU access window.
   *
   * Otherwise set write in progress flag, and return here later.
   */
  if(mmu->waitstate_counter <= 0 && (area->mmu_protected == MMU_NOT_PROTECTED || mmu_in_cpu_window(mmu))) {
    area->write_byte(area->data, addr, data);
    mmu->cpu->external->data_available = 1;
  } else {
    mmu->write_in_progress = 1;
  }
}

void mmu_write_word(struct mmu *mmu) {
  unused(mmu);
  /* TODO: All */
}

WORD mmu_peek_word(struct mmu *mmu, LONG addr) {
  struct mmu_area *area;

  area = mmu->areas[addr&0xffffff];
  return area->peek_word(area->data, addr&0xffffff);
}


/* Register a memory segment for a particular hardware object 
 * This builds the memory map
 */
void mmu_register_area(struct mmu *mmu, LONG start, LONG size, struct mmu_area *area) {
  LONG i;

  start = start&0xffffff;
  
  for(i=start;i<start+size;i++) {
    mmu->areas[i] = area;
  }
}

/* Fallback function for peek_word in case hardware does not define one. 
 * This function must never cause an error, so it just returns a static value.
 */
static WORD mmu_fallback_peek_word(void *data, LONG addr) {
  unused(data);
  unused(addr);
  return 0x2a2a;
}

static struct mmu_area *default_mmu_area() {
  struct mmu_area *area;
  
  area = (struct mmu_area *)ostis_alloc(sizeof(struct mmu_area));
  area->peek_word = mmu_fallback_peek_word;
  return area;
}

/* Helper function to create an area for registration 
 */
struct mmu_area *mmu_create_area(read_byte_t *read_byte, read_word_t *read_word,
                                 write_byte_t *write_byte, write_word_t *write_word,
                                 read_word_t *peek_word,
                                 void *data, enum mmu_protection protected,
                                 int waitstates) {
  struct mmu_area *area;
  area = default_mmu_area();
  
  if(read_byte)  { area->read_byte  = read_byte; }
  if(read_word)  { area->read_word  = read_word; }
  if(write_byte) { area->write_byte = write_byte; }
  if(write_word) { area->write_word = write_word; }
  if(peek_word)  { area->peek_word  = peek_word; }
  area->data = data;
  area->mmu_protected = protected;
  area->waitstates = waitstates;

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
  mmu->tick_alignment = 0;

  return mmu;
}

void mmu_clear_read_progress(struct mmu *mmu) {
  mmu->read_in_progress = 0;
}

void mmu_clear_write_progress(struct mmu *mmu) {
  mmu->write_in_progress = 0;
}

void mmu_tick(struct hw *hw) {
  struct mmu *mmu;
  mmu = hw->data;

  /* Check if there is a read in progress (waitstate)
   * If so, check if MMU can allow CPU read to finish.
   * TODO: Write stuff.
   */
  if(mmu->read_in_progress) {
    /* Decrease waitstate counter and do nothing, or tell reader that data is available */
    if(mmu->waitstate_counter > 0) {
      printf("DEBUG-MMU-READ: Decreasing waitstate counter: %d\n", mmu->waitstate_counter);
      mmu->waitstate_counter--;
    } else {
      if(mmu_in_cpu_window(mmu)) {
        printf("DEBUG: Making data available\n");
        mmu->cpu->external->data_available = 1;
      }
    }
  }

  if(mmu->write_in_progress) {
    /* Decrease waitstate counter and do nothing, or perform actual write */
    if(mmu->waitstate_counter > 0) {
      printf("DEBUG-MMU-WRITE: Decreasing waitstate counter: %d\n", mmu->waitstate_counter);
      mmu->waitstate_counter--;
    } else {
      if(mmu_in_cpu_window(mmu)) {
        printf("DEBUG: Actually writing data\n");
        mmu_write_byte(mmu);
        mmu->cpu->external->data_available = 1;
      }
    }
  }
  
  printf("DEBUG: Ticking MMU... (%d => %d)\n", mmu->tick_alignment, mmu_in_cpu_window(mmu));
  mmu->tick_alignment++;
}
