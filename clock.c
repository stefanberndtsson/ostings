#include <stdint.h>
#include <unistd.h>
#include "hw.h"

/* Adjusted to match 160256 cycles per 50Hz video frame */
#define MASTER_FREQ 32051200

/* CPU is 8MHz */
#define CPU_DIVIDER 4

/* MMU is 16MHz */
#define MMU_DIVIDER 2

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
    if((clock & (MMU_DIVIDER-1)) == 0) {
      send_tick(hw[HW_MMU]);
    }
    if((clock & (CPU_DIVIDER-1)) == 0) {
      send_tick(hw[HW_CPU]);
    }
    clock++;
    //usleep(250000);
  }
}


