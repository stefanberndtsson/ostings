#include "common.h"
#include "mmu.h"
#include "psg.h"

#define PSGSTART 0xff8800
#define PSGSIZE 256

/* TODO: All actual PSG stuff. For the moment it is just another piece of memory */

static BYTE psg_read_byte(struct psg *psg, LONG addr) {
  return psg->memory[addr-PSGSTART];
}

static WORD psg_read_word(struct psg *psg, LONG addr) {
  return (psg_read_byte(psg, addr)<<8)|psg_read_byte(psg, addr+1);
}

static void psg_write_byte(struct psg *psg, LONG addr, BYTE value) {
  psg->memory[addr-PSGSTART] = value;
}

static void psg_write_word(struct psg *psg, LONG addr, WORD value) {
  psg_write_byte(psg, addr, value>>8);
  psg_write_byte(psg, addr, value&0xff);
}

static WORD psg_peek_word(struct psg *psg, LONG addr) {
  return psg_read_word(psg, addr);
}

struct psg *psg_setup(struct hw **hws) {
  struct psg *psg;
  struct mmu_area *area;

  psg = (struct psg *)ostis_alloc(sizeof(struct psg));
  psg->hws = hws;
  psg->memory = (BYTE *)ostis_alloc(PSGSIZE);
  psg->start = PSGSTART;
  psg->size = PSGSIZE;
  psg->end = PSGSTART+PSGSIZE-1;

  area = mmu_create_area((read_byte_t *)psg_read_byte,
                         (read_word_t *)psg_read_word,
                         (write_byte_t *)psg_write_byte,
                         (write_word_t *)psg_write_word,
                         (read_word_t *)psg_peek_word,
                         psg, MMU_PROTECTED, 4);
  mmu_register_area(hws[HW_MMU]->data, psg->start, psg->size, area);
  
  return psg;
}
