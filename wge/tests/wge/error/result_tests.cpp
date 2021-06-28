#include <wge/error/result.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeErrorResult);

WTEST_SETUP(WgeErrorResult) {}
WTEST_TEAR_DOWN(WgeErrorResult) {}

namespace {



}  // namespace

WTEST_CASE(WgeErrorResult, construction_with_value) {
    using result_type      = wge::result_t<wge::u32, wge::u32>;
    constexpr wge::u32 val = 145;

    const result_type result(val);
    WTEST_ASSERT_TRUE(result.has_value());
    WTEST_ASSERT_FALSE(result.has_error());

    WTEST_ASSERT_EQUAL(val, result.value());
}


WTEST_SUITE_RUNNER(WgeErrorResult) {
    RUN_TEST_CASE(WgeErrorResult, construction_with_value);
}
