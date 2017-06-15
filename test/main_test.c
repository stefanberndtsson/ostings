#include "cutest.h"

test(main_shall_set_up_hardware) {
  MAIN(0, NULL);
  assert_eq(1, cutest_mock.hw_setup.call_count);
}

test(main_shall_start_clock) {
  MAIN(0, NULL);
  assert_eq(1, cutest_mock.clock_run.call_count);
}

test(main_shall_return_0_if_all_is_ok) {
  assert_eq(0, MAIN(0, NULL));
}
