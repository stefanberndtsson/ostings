#include <stdio.h>
#include "clock.h"
#include "unused.h"
#include "hw.h"

int global_debug = 0;

int main(int argc, char *argv[]) {
  unused(argc);
  unused(argv);
  struct hw **hw;
  hw = hw_setup();
  clock_run(hw);
}
