#include <wge/span.hpp>

#include <wtest/wtest.hpp>

#include <array>

WTEST_SUITE(WgeSpan);

WTEST_SETUP(WgeSpan) {}
WTEST_TEAR_DOWN(WgeSpan) {}

WTEST_CASE(WgeSpan, default_construction) {
    wge::span<wge::u16, 10U> test_span{};
    WTEST_ASSERT_TRUE(test_span.empty());
    WTEST_ASSERT_EQUAL(static_cast<std::size_t>(0U), test_span.size());
}

WTEST_CASE(WgeSpan, construction_from_array_types) {
    {
        constexpr wge::size_t array_size = 5U;
        const std::array<wge::u32, array_size> test_array{};
        wge::span<const wge::u32> test_span(test_array);

        WTEST_ASSERT_FALSE(test_span.empty());
        WTEST_ASSERT_EQUAL(array_size, test_span.size());

        constexpr wge::size_t byte_size = test_array.size() * sizeof( decltype(test_array)::value_type );
        WTEST_ASSERT_EQUAL(byte_size, test_span.size_byte());

    }
}

WTEST_SUITE_RUNNER(WgeSpan) {
    RUN_TEST_CASE(WgeSpan, default_construction);
    RUN_TEST_CASE(WgeSpan, construction_from_array_types);
}
