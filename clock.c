#include <stdint.h>
#include <unistd.h>
#include "hw.h"

/* Adjusted to match 160256 cycles per 50Hz video frame */
#define MASTER_FREQ 32051200

/* While CPU is technically 8MHz, it has 2 states per cycle internally */
#define CPU_DIVIDER 2

static void send_tick(struct hw *hw) {
  if(hw->tick) {
    hw->tick(hw);
  }
}

/* Main run loop. Fairly hardcoded for now. */
void clock_run(struct hw **hw) {
  uint64_t clock = 0;
  while(1) {
    send_tick(hw[HW_SHIFTER]);
    if(clock & (CPU_DIVIDER-1)) {
      send_tick(hw[HW_CPU]);
    }
    clock++;
    sleep(1);
  }
}


