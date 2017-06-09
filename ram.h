#ifndef OSTIS_RAM_H
#define OSTIS_RAM_H

#include "common.h"
#include "hw.h"

/* struct ram:
 * Fairly generic memory area struct
 * 
 * Start:  Memory address where area resides
 * Size:   Size of area in bytes
 * End:    Last byte of area
 * Memory: Actual memory data
 * 
 */
struct ram {
  LONG start;
  LONG size;
  LONG end;
  BYTE *memory;

  struct hw **hws;
};

struct ram *ram_setup(struct hw **);

#endif /* OSTIS_RAM_H */
