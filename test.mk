Q=

CUTEST_TEST_DIR=./test

all::
	test -f $(PRG) && make -s cutest && make -s check

cutest:
	git clone "https://github.com/aiobofh/cutest.git" -q

-include cutest/src/cutest.mk
-include cutest/src/coverage.mk

$(CUTEST_TEST_DIR)/main_test: $(filter-out main.c,$(SRC)) $(LIB)
