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

struct destruction_track {
    destruction_track(wge::u32& t) : tracker(t) {}
    ~destruction_track() noexcept { tracker++; }

    wge::u32& tracker;
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

WTEST_CASE(WgeErrorResult, construction_with_move_value) {
    using value_type       = none_copyable<wge::u32>;
    using result_type      = wge::result_t<value_type, wge::u32>;
    constexpr wge::u32 val = 145;

    result_type result{value_type(val)};

    WTEST_ASSERT_TRUE(result.has_value());
    WTEST_ASSERT_FALSE(result.has_error());

    auto result_val = std::move(result.value());
    WTEST_ASSERT_EQUAL(val, result_val.val);
}

WTEST_CASE(WgeErrorResult, construction_with_error) {
    using result_type      = wge::result_t<wge::u32, wge::u32>;
    constexpr wge::u32 val = 145;

    const result_type result = wge::make_error_result(val);
    WTEST_ASSERT_FALSE(result.has_value());
    WTEST_ASSERT_TRUE(result.has_error());

    WTEST_ASSERT_EQUAL(val, result.error());
}

WTEST_CASE(WgeErrorResult, construction_with_move_error) {
    using error_type       = none_copyable<wge::u32>;
    using result_type      = wge::result_t<wge::u32, error_type>;
    constexpr wge::u32 val = 145;

    result_type result = wge::make_error_result(error_type(val));

    WTEST_ASSERT_FALSE(result.has_value());
    WTEST_ASSERT_TRUE(result.has_error());

    auto result_err = std::move(result.error());
    WTEST_ASSERT_EQUAL(val, result_err.val);
}

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
}

WTEST_CASE(WgeErrorResult, value_or_alternative) {
    using result_type = wge::result_t<char, char>;

    {
        const result_type result('A');
        WTEST_ASSERT_EQUAL('A', result.value_or('B'));
    }

    {
        const result_type result = wge::make_error_result('E');
        WTEST_ASSERT_EQUAL('Z', result.value_or('Z'));
    }
}

WTEST_CASE(WgeErrorResult, value_is_destructed) {
    using result_type = wge::result_t<destruction_track, char>;

    wge::u32 tracker = 0;
    {
        const result_type result{destruction_track(tracker)};
        WTEST_ASSERT_EQUAL(2U, tracker);
    }
    WTEST_ASSERT_EQUAL(3U, tracker);
}

WTEST_CASE(WgeErrorResult, error_is_destructed) {
    using result_type = wge::result_t<char, destruction_track>;

    wge::u32 tracker = 0;
    {
        const result_type result = wge::make_error_result(destruction_track(tracker));
        WTEST_ASSERT_EQUAL(2U, tracker);
    }
    WTEST_ASSERT_EQUAL(3U, tracker);
}

WTEST_SUITE_RUNNER(WgeErrorResult) {
    RUN_TEST_CASE(WgeErrorResult, construction_with_value);
    RUN_TEST_CASE(WgeErrorResult, construction_with_error);
    RUN_TEST_CASE(WgeErrorResult, construction_with_move_value);
    RUN_TEST_CASE(WgeErrorResult, construction_with_move_error);

    RUN_TEST_CASE(WgeErrorResult, value_is_destructed);
    RUN_TEST_CASE(WgeErrorResult, error_is_destructed);
    
    RUN_TEST_CASE(WgeErrorResult, value_access_options);
    RUN_TEST_CASE(WgeErrorResult, value_or_alternative);
}
