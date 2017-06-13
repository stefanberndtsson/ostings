#ifndef OSTIS_CART_H
#define OSTIS_CART_H

#include "common.h"
#include "hw.h"

/* struct cart:
 * Fairly generic memory area struct
 * 
 * Start:  Memory address where area resides
 * Size:   Size of area in bytes
 * End:    Last byte of area
 * Memory: Actual memory data
 * 
 */
struct cart {
  LONG start;
  LONG size;
  LONG end;
  BYTE *memory;

  struct hw **hws;
};

struct cart *cart_setup(struct hw **);

#endif /* OSTIS_CART_H */
