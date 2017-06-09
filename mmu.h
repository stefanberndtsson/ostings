#ifndef OSTIS_MMU_H
#define OSTIS_MMU_H

#include "common.h"
#include "hw.h"

#define MAX_AREAS ((0xffffff)+1)

typedef BYTE read_byte_t(void *, LONG);
typedef WORD read_word_t(void *, LONG);
typedef void write_byte_t(void *, LONG, BYTE);
typedef void write_word_t(void *, LONG, WORD);

/* enum mmu_protection:
 *
 * NOT_PROTECTED
 * PROTECTED
 */
enum mmu_protection {
  MMU_NOT_PROTECTED=0,
  MMU_PROTECTED
};

/* struct mmu_area:
 * A segment of memory, RAM, ROM, and so on, with its corresponding read/write functions
 *
 * MMU protected: Flag indicating whether or not to return data immediately, or align to 4 cycles with MMU
 * Data:          Data specific to actual area
 * 
 * Read Byte:     Read BYTE from area
 * Read Word:     Read WORD from area
 * Write Byte:    Write BYTE to area
 * Write Word:    Write WORD to area
 */
struct mmu_area {
  int mmu_protected;
  void *data;

  read_byte_t *read_byte;
  read_word_t *read_word;
  write_byte_t *write_byte;
  write_word_t *write_word;
};

/* struct mmu:
 * The actual memory map
 * 
 * Areas: Array of 65536 address locations
 * CPU:   Reference to CPU for setting/getting Data and DTACK
 */
struct mmu {
  struct mmu_area *areas[MAX_AREAS];
  struct hw **hws;
  struct cpu *cpu;
};

void mmu_tick(struct hw *);
struct mmu *mmu_setup(struct hw **);
struct mmu_area *mmu_create_area(void *, void *, void *, void *, void *, enum mmu_protection);
void mmu_register_area(struct mmu *, LONG, LONG, struct mmu_area *);
void mmu_read_byte(struct mmu *);
void mmu_read_word(struct mmu *);
void mmu_write_byte(struct mmu *);
void mmu_write_word(struct mmu *);

#endif /* OSTIS_MMU_H */
