#include "common.h"
#include "mmu.h"
#include "mfp.h"

#define MFPSTART 0xfffa00
#define MFPSIZE 64

/* TODO: All actual MFP stuff. For the moment it is just another piece of memory */

static BYTE mfp_read_byte(struct mfp *mfp, LONG addr) {
  return mfp->memory[addr-MFPSTART];
}

static WORD mfp_read_word(struct mfp *mfp, LONG addr) {
  return (mfp_read_byte(mfp, addr)<<8)|mfp_read_byte(mfp, addr+1);
}

static void mfp_write_byte(struct mfp *mfp, LONG addr, BYTE value) {
  mfp->memory[addr-MFPSTART] = value;
}

static void mfp_write_word(struct mfp *mfp, LONG addr, WORD value) {
  mfp_write_byte(mfp, addr, value>>8);
  mfp_write_byte(mfp, addr, value&0xff);
}

static WORD mfp_peek_word(struct mfp *mfp, LONG addr) {
  return mfp_read_word(mfp, addr);
}

struct mfp *mfp_setup(struct hw **hws) {
  struct mfp *mfp;
  struct mmu_area *area;

  mfp = (struct mfp *)ostis_alloc(sizeof(struct mfp));
  mfp->hws = hws;
  mfp->memory = (BYTE *)ostis_alloc(MFPSIZE);
  mfp->start = MFPSTART;
  mfp->size = MFPSIZE;
  mfp->end = MFPSTART+MFPSIZE-1;

  area = mmu_create_area((read_byte_t *)mfp_read_byte,
                         (read_word_t *)mfp_read_word,
                         (write_byte_t *)mfp_write_byte,
                         (write_word_t *)mfp_write_word,
                         (read_word_t *)mfp_peek_word,
                         mfp, MMU_PROTECTED, 4);
  mmu_register_area(hws[HW_MMU]->data, mfp->start, mfp->size, area);
  
  return mfp;
}

/* Just accept ticks, but do nothing for now */
void mfp_tick(struct hw *hw) {
  unused(hw);
}
