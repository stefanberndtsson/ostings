#ifndef OSTIS_HW_H
#define OSTIS_HW_H

#include "clock.h"

/* TODO: Incomplete */
/* Hardware components as values */
enum hw_components {
  HW_CPU,
  HW_SHIFTER,
  HW_GLUE,
  HW_MMU,
  HW_PSG,
  HW_RAM,
  HW_ROM,
  HW_BOOTROM,
  HW_COUNT
};

struct hw {
  enum hw_components id;
  tick *tick;
  void *data;
};

struct hw **hw_setup();

#endif /* OSTIS_HW_H */
