#ifndef OSTIS_PSG_H
#define OSTIS_PSG_H

#include "common.h"
#include "hw.h"

/* struct psg:
 * Fairly generic memory area struct.
 * TODO: Extend with PSG related things.
 * 
 * Start:  Memory address where area resides
 * Size:   Size of area in bytes
 * End:    Last byte of area
 * Memory: Actual memory data
 * 
 */
struct psg {
  LONG start;
  LONG size;
  LONG end;
  BYTE *memory;

  struct hw **hws;
};

struct psg *psg_setup(struct hw **);

#endif /* OSTIS_PSG_H */
