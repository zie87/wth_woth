#include <wge/utils/defer.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeUtilsDefer);

WTEST_SETUP(WgeUtilsDefer) {}
WTEST_TEAR_DOWN(WgeUtilsDefer) {}

WTEST_CASE(WgeUtilsDefer, defer_calls_function_on_destruction) {
    wge::u32 counter = 0;
    {
        wge_defer { counter++; };
        WTEST_ASSERT_EQUAL(0U, counter);
    }
    WTEST_ASSERT_EQUAL(1U, counter);
}

WTEST_SUITE_RUNNER(WgeUtilsDefer) { RUN_TEST_CASE(WgeUtilsDefer, defer_calls_function_on_destruction); }
