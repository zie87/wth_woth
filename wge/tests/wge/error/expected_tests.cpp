#include <wge/error/expected.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeErrorExpected);

WTEST_SETUP(WgeErrorExpected) {}
WTEST_TEAR_DOWN(WgeErrorExpected) {}

WTEST_CASE(WgeErrorExpected, construct_default_expected) {
    using expected_type = wge::expected<wge::i32, char>;

    expected_type exp{};
    WTEST_ASSERT_TRUE(exp.has_value());
}

WTEST_CASE(WgeErrorExpected, construct_with_same_expected_type) {
    using expected_type = wge::expected<wge::i32, char>;
}

WTEST_SUITE_RUNNER(WgeErrorExpected) {
    RUN_TEST_CASE(WgeErrorExpected, construct_default_expected);
    RUN_TEST_CASE(WgeErrorExpected, construct_with_same_expected_type);
}
