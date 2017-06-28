#include <stdint.h>
#include <unistd.h>
#include "hw.h"

/* Adjusted to match 160256 cycles per 50Hz video frame */
#define MASTER_FREQ 32051200

/* CPU is 8MHz */
#define CPU_DIVIDER 4

/* MMU is 16MHz */
#define MMU_DIVIDER 2

/* MFP has a separate clock of 2.4576MHz
 * Because of this, it is not synced with the system clock,
 * but here it will be, more or less.
 * Every 13 ticks should send an MFP-tick, except after
 * 23 MFP-ticks, it should be 14 ticks before the next MFP-tick.
 * That should keep it at a proper frequency.
 */
#define MFP_DIVIDER 13
#define MFP_DELAY_AFTER 23

static void send_tick(struct hw *hw) {
  if(hw->tick) {
    hw->tick(hw);
  }
}

/* Main run loop. Fairly hardcoded for now. */
void clock_run(struct hw **hw) {
  uint64_t clock = 0;
  int mfp_divider = MFP_DIVIDER;
  int mfp_delay_counter = 0;
  while(1) {
    send_tick(hw[HW_SHIFTER]);
    if((clock & (MMU_DIVIDER-1)) == 0) {
      send_tick(hw[HW_MMU]);
    }
    if((clock & (CPU_DIVIDER-1)) == 0) {
      send_tick(hw[HW_CPU]);
    }
    if((clock%mfp_divider) == 0) {
      send_tick(hw[HW_MFP]);
      mfp_divider = MFP_DIVIDER;
      mfp_delay_counter++;
      if(mfp_delay_counter > MFP_DELAY_AFTER) {
        mfp_divider = MFP_DIVIDER+1;
        mfp_delay_counter = 0;
      }
    }
    clock++;
    //usleep(250000);
  }
}


