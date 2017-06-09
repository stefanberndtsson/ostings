#ifndef OSTIS_BOOTROM_H
#define OSTIS_BOOTROM_H

#include "common.h"
#include "hw.h"

/* struct bootrom:
 * Fairly generic memory area struct
 * 
 * Start:  Memory address where area resides
 * Size:   Size of area in bytes
 * End:    Last byte of area
 * Memory: Actual memory data
 * 
 */
struct bootrom {
  LONG start;
  LONG size;
  LONG end;
  BYTE *memory;

  struct hw **hws;
};

struct bootrom *bootrom_setup(struct hw **);

#endif /* OSTIS_BOOTROM_H */
