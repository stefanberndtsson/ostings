#ifndef OSTIS_ROM_H
#define OSTIS_ROM_H

#include "common.h"
#include "hw.h"

/* struct rom:
 * Fairly generic memory area struct
 * 
 * Start:  Memory address where area resides
 * Size:   Size of area in bytes
 * End:    Last byte of area
 * Memory: Actual memory data
 * 
 */
struct rom {
  LONG start;
  LONG size;
  LONG end;
  BYTE *memory;

  struct hw **hws;
};

struct rom *rom_setup(struct hw **);

#endif /* OSTIS_ROM_H */
