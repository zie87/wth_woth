#include <wge/error/expected.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeErrorExpected);

WTEST_SETUP(WgeErrorExpected) {}
WTEST_TEAR_DOWN(WgeErrorExpected) {}

WTEST_CASE(WgeErrorExpected, nearest_superior_power_of_2) {

}


WTEST_SUITE_RUNNER(WgeErrorExpected) {
    RUN_TEST_CASE(WgeErrorExpected, nearest_superior_power_of_2);
}
