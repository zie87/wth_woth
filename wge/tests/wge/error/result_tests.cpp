#include <wge/error/result.hpp>

#include <wtest/wtest.hpp>

WTEST_SUITE(WgeErrorResult);

WTEST_SETUP(WgeErrorResult) {}
WTEST_TEAR_DOWN(WgeErrorResult) {}

namespace {

template <typename T>
struct none_copyable {
    explicit none_copyable(const T& v) : val(v){};
    ~none_copyable() noexcept = default;

    none_copyable(const none_copyable&) = delete;
    none_copyable& operator=(const none_copyable&) = delete;

    none_copyable(none_copyable&&) noexcept = default;
    none_copyable& operator=(none_copyable&&) noexcept = default;

    T val;
};
}  // namespace

WTEST_CASE(WgeErrorResult, construction_with_value) {
    using result_type      = wge::result_t<wge::u32, wge::u32>;
    constexpr wge::u32 val = 145;

    const result_type result(val);
    WTEST_ASSERT_TRUE(result.has_value());
    WTEST_ASSERT_FALSE(result.has_error());

    WTEST_ASSERT_EQUAL(val, result.value());
}

WTEST_CASE(WgeErrorResult, construction_with_error) {
    using result_type      = wge::result_t<wge::u32, wge::u32>;
    constexpr wge::u32 val = 145;

    const result_type result = wge::make_error_result(val);
    WTEST_ASSERT_FALSE(result.has_value());
    WTEST_ASSERT_TRUE(result.has_error());

    WTEST_ASSERT_EQUAL(val, result.error());
}


// WTEST_CASE(WgeErrorResult, construction_with_move_value) {
//     using value_type = none_copyable<wge::u32>;
//     using result_type      = wge::result_t<value_type, wge::u32>;
//     constexpr wge::u32 val = 145;
//
//     result_type result{value_type(val)};
//
//     WTEST_ASSERT_TRUE(result.has_value());
//     WTEST_ASSERT_FALSE(result.has_error());
//
//     auto result_val = result.value();
//     WTEST_ASSERT_EQUAL(val, result.value().val);
// }

WTEST_CASE(WgeErrorResult, value_access_options) {
    using result_type      = wge::result_t<wge::u32, wge::u32>;
    constexpr wge::u32 val = 145;

    // const reference
    {
        const result_type result(val);
        WTEST_ASSERT_EQUAL(val, result.value());
    }

    // mutable reference
    {
        result_type result(val);
        constexpr wge::u32 new_val = 42U;

        result.value() = new_val;
        WTEST_ASSERT_EQUAL(new_val, result.value());
    }

    // move value out
    {
        result_type result(val);
        const auto ret_val = std::move(result.value());
        WTEST_ASSERT_EQUAL(val, ret_val);
    }
}

WTEST_SUITE_RUNNER(WgeErrorResult) {
    RUN_TEST_CASE(WgeErrorResult, construction_with_value);
    RUN_TEST_CASE(WgeErrorResult, construction_with_error);
    RUN_TEST_CASE(WgeErrorResult, value_access_options);
}
