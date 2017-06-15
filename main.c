#include <stdio.h>
#include "unused.h"
#include "clock.h"
#include "hw.h"

int main(int argc, char *argv[]) {
  unused(argc);
  unused(argv);
  struct hw **hw;
  hw = hw_setup();
  clock_run(hw);
  return 0;
}
