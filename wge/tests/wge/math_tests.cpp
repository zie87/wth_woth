#include <wge/math.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeMath);

WTEST_SETUP(WgeMath) {}
WTEST_TEAR_DOWN(WgeMath) {}

WTEST_CASE(WgeMath, nearest_superior_power_of_2) {
    {
        constexpr wge::u32 val = 145;
        const auto result = wge::math::nearest_superior_power_of_2(val);
        WTEST_ASSERT_EQUAL(256U, result);
    }

}


WTEST_SUITE_RUNNER(WgeMath) {
    RUN_TEST_CASE(WgeMath, nearest_superior_power_of_2);
}
