#include "common.h"
#include "mmu.h"
#include "cart.h"

#define CARTSTART 0x00fa0000
#define CARTSIZE 131072
#define CARTFILE "cart.img"

static void dummy_fill_cart(struct cart *cart) {
  FILE *fp;

  fp = fopen(CARTFILE, "rb");
  if(!fp) {
    return;
  }
  fread(cart->memory, 1, CARTSIZE, fp);
  fclose(fp);
}

static BYTE cart_read_byte(struct cart *cart, LONG addr) {
  return cart->memory[addr-CARTSTART];
}

static WORD cart_read_word(struct cart *cart, LONG addr) {
  return (cart_read_byte(cart, addr)<<8)|cart_read_byte(cart, addr+1);
}

static WORD cart_peek_word(struct cart *cart, LONG addr) {
  return cart_read_word(cart, addr);
}

struct cart *cart_setup(struct hw **hws) {
  struct cart *cart;
  struct mmu_area *area;

  cart = (struct cart *)ostis_alloc(sizeof(struct cart));
  cart->hws = hws;
  cart->memory = (BYTE *)ostis_alloc(CARTSIZE);
  cart->start = CARTSTART;
  cart->size = CARTSIZE;
  cart->end = CARTSTART+CARTSIZE-1;

  dummy_fill_cart(cart);
  
  area = mmu_create_area((read_byte_t *)cart_read_byte,
                         (read_word_t *)cart_read_word,
                         NULL, NULL,
                         (read_word_t *)cart_peek_word,
                         cart, MMU_NOT_PROTECTED);
  mmu_register_area(hws[HW_MMU]->data, cart->start, cart->size, area);
  
  return cart;
}