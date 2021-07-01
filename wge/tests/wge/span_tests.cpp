#include <wge/span.hpp>

#include <wtest/wtest.hpp>

#include <algorithm>
#include <numeric>

WTEST_SUITE(WgeSpan);

WTEST_SETUP(WgeSpan) {}
WTEST_TEAR_DOWN(WgeSpan) {}

WTEST_CASE(WgeSpan, default_construction) {
    wge::span<wge::u16, 10U> test_span{};
    WTEST_ASSERT_TRUE(test_span.empty());
    WTEST_ASSERT_EQUAL(static_cast<std::size_t>(0U), test_span.size());
}

WTEST_CASE(WgeSpan, construct_from_array) {
    constexpr wge::size_t array_size = 25;
    wge::byte_t test_array[array_size];

    wge::span<wge::byte_t> test_span(test_array);
    WTEST_ASSERT_EQUAL(array_size, test_span.size());
}

WTEST_CASE(WgeSpan, construct_from_array_class) {
    {
        constexpr wge::size_t array_size = 5U;
        const std::array<wge::u32, array_size> test_array{};
        wge::span<const wge::u32> test_span(test_array);

        WTEST_ASSERT_FALSE(test_span.empty());
        WTEST_ASSERT_EQUAL(array_size, test_span.size());

        constexpr wge::size_t byte_size = test_array.size() * sizeof(decltype(test_array)::value_type);
        WTEST_ASSERT_EQUAL(byte_size, test_span.size_byte());
    }

    {
        constexpr wge::size_t array_size = 10U;
        std::array<wge::f32, array_size> test_array{};
        wge::span<const wge::f32> test_span(test_array);

        WTEST_ASSERT_FALSE(test_span.empty());
        WTEST_ASSERT_EQUAL(array_size, test_span.size());
    }
}

WTEST_CASE(WgeSpan, iterators_and_algorithms) {
    {
        constexpr std::array test_array{1, 2, 3, 4, 5, 6};
        using span_type = wge::span<const decltype(test_array)::value_type, test_array.size()>;
        span_type test_span{test_array};

        const auto value = std::accumulate(test_span.begin(), test_span.end(), 0);
        WTEST_ASSERT_EQUAL(21, value);
    }
    {
        constexpr std::array test_array{1, 2, 3, 4, 5, 6};
        using span_type = wge::span<const decltype(test_array)::value_type, test_array.size()>;
        span_type test_span{test_array};

        const auto it = std::max_element(test_span.rbegin(), test_span.rend());
        WTEST_ASSERT_EQUAL(test_span.back(), *it);
    }
}

WTEST_CASE(WgeSpan, subspan_getter) {
    using value_type                 = wge::i16;
    constexpr wge::size_t array_size = 10;

    using array_type = std::array<value_type, array_size>;
    using span_type  = wge::span<array_type::value_type, array_size>;

    constexpr array_type expected{1, 1, 1, 2, 2, 2, 2, 3, 3, 3};

    {
        array_type test_array{};
        span_type test_span(test_array.begin(), test_array.end());

        {
            auto first_part = test_span.first(3);
            std::fill(first_part.begin(), first_part.end(), 1);
        }

        {
            auto middle_part = test_span.subspan(3, 4);
            std::fill(middle_part.rbegin(), middle_part.rend(), 2);
        }

        {
            auto last_part = test_span.last(3);
            std::fill(last_part.rbegin(), last_part.rend(), 3);
        }

        for (wge::size_t idx = 0; idx < array_size; ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], test_span[idx]);
        }
    }

    {
        array_type test_array{};
        span_type test_span(test_array.begin(), test_array.end());

        {
            auto first_part = test_span.first<3>();
            std::fill(first_part.begin(), first_part.end(), 1);
        }

        {
            auto middle_part = test_span.subspan<3, 4>();
            std::fill(middle_part.rbegin(), middle_part.rend(), 2);
        }

        {
            auto last_part = test_span.last<3>();
            std::fill(last_part.rbegin(), last_part.rend(), 3);
        }

        for (wge::size_t idx = 0; idx < array_size; ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], test_span[idx]);
        }
    }
}

WTEST_SUITE_RUNNER(WgeSpan) {
    RUN_TEST_CASE(WgeSpan, default_construction);
    RUN_TEST_CASE(WgeSpan, construct_from_array);
    RUN_TEST_CASE(WgeSpan, construct_from_array_class);
    RUN_TEST_CASE(WgeSpan, iterators_and_algorithms);
    RUN_TEST_CASE(WgeSpan, subspan_getter);
}
