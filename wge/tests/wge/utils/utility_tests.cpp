#include <wge/utils/utility.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeUtilsUtility);

WTEST_SETUP(WgeUtilsUtility) {}
WTEST_TEAR_DOWN(WgeUtilsUtility) {}

WTEST_CASE(WgeUtilsUtility, defer_calls_function_on_destruction) {
    wge::u32 counter = 0;
    {
        wge_defer { counter++; };
        WTEST_ASSERT_EQUAL(0U, counter);
    }
    WTEST_ASSERT_EQUAL(1U, counter);
}

WTEST_SUITE_RUNNER(WgeUtilsUtility) { RUN_TEST_CASE(WgeUtilsUtility, defer_calls_function_on_destruction); }
