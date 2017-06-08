#include <stdio.h>
#include "clock.h"
#include "hw.h"

int main(int argc, char *argv[]) {
  struct hw **hw;
  hw = hw_setup();
  clock_run(hw);
}
