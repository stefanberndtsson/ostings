#ifndef OSTIS_MFP_H
#define OSTIS_MFP_H

#include "common.h"
#include "hw.h"

/* struct psg:
 * Fairly generic memory area struct.
 * TODO: Extend with MFP related things.
 * 
 * Start:  Memory address where area resides
 * Size:   Size of area in bytes
 * End:    Last byte of area
 * Memory: Actual memory data
 * 
 */
struct mfp {
  LONG start;
  LONG size;
  LONG end;
  BYTE *memory;

  struct hw **hws;
};

struct mfp *mfp_setup(struct hw **);

#endif /* OSTIS_MFP_H */
