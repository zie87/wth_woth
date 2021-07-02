#include <wge/error/unexpected.hpp>
#include <wge/types.hpp>

#include <wtest/wtest.hpp>

#include <string>

WTEST_SUITE(WgeErrorUnexpected);

WTEST_SETUP(WgeErrorUnexpected) {}
WTEST_TEAR_DOWN(WgeErrorUnexpected) {}

template <typename T>
struct not_copyable {
    explicit not_copyable(const T& v) noexcept : value(v) {}

    not_copyable(const not_copyable&) = delete;
    not_copyable& operator=(const not_copyable&) = delete;

    not_copyable(not_copyable&&) = default;
    not_copyable& operator=(not_copyable&&) = default;

    T value;
};

template <typename T, typename U>
struct simple_pair {
    simple_pair(const T& f, const U& s) noexcept : first(f), second(s) {}

    T first;
    U second;
};

WTEST_CASE(WgeErrorUnexpected, construct_unexpected) {
    {
        using error_type = not_copyable<wge::u32>;
        wge::unexpected<error_type> unexpected(12);

        const auto ret = std::move(unexpected.value());
        WTEST_ASSERT_EQUAL(12U, ret.value);
    }

    {
        using error_type = simple_pair<wge::u32, wge::f32>;
        const wge::unexpected<error_type> unexpected(std::in_place, 42U, 1337.F);

        const auto ret_pair = unexpected.value();
        WTEST_ASSERT_EQUAL(42U, ret_pair.first);
        WTEST_ASSERT_EQUAL(1337.F, ret_pair.second);
    }

    {
        using error_type = std::string;
        const wge::unexpected<error_type> unexpected(std::in_place, {'a', 'b', 'c'});

        const auto ret_str = unexpected.value();
        WTEST_ASSERT_EQUAL(std::string("abc"), ret_str);
    }
}

WTEST_CASE(WgeErrorUnexpected, construct_unexpected_with_unexpected) {
    {
        using unexpected_v1 = wge::unexpected<wge::u16>;
        using unexpected_v2 = wge::unexpected<wge::i32>;

        const unexpected_v1 tmp(17);
        unexpected_v2 unexp(tmp);
        WTEST_ASSERT_EQUAL(17, unexp.value());
    }
    {
        using unexpected_v1 = wge::unexpected<wge::u16>;
        using unexpected_v2 = wge::unexpected<not_copyable<wge::i32>>;

        unexpected_v2 unexp(unexpected_v1(17));

        const auto ret = std::move(unexp.value().value);
        WTEST_ASSERT_EQUAL(17, ret);
    }
}

WTEST_CASE(WgeErrorUnexpected, assigne_other_unexpected) {
    {
        using unexpected_v1 = wge::unexpected<wge::u16>;
        using unexpected_v2 = wge::unexpected<wge::i32>;

        const unexpected_v1 unexp1(43);
        unexpected_v2 unexp2(123);

        unexp2 = unexp1;
        WTEST_ASSERT_EQUAL(43, unexp2.value());
    }
    {
        using unexpected_v1 = wge::unexpected<wge::i16>;
        using unexpected_v2 = wge::unexpected<wge::i32>;

        unexpected_v2 unexp(123);
        unexp = unexpected_v2(23);
        WTEST_ASSERT_EQUAL(23, unexp.value());
    }
}

WTEST_CASE(WgeErrorUnexpected, equality_of_unexpected) {
    {
        using unexpected_v1 = wge::unexpected<wge::u16>;
        using unexpected_v2 = wge::unexpected<wge::i32>;

        const unexpected_v1 unexp1(43);
        const unexpected_v2 unexp2(43);

        WTEST_ASSERT_TRUE(unexp1 == unexp2);
    }
    {
        using unexpected_v1 = wge::unexpected<wge::i16>;
        using unexpected_v2 = wge::unexpected<wge::i32>;

        unexpected_v1 unexp1(17);
        unexpected_v2 unexp2(23);
        WTEST_ASSERT_TRUE(unexp1 != unexp2);
    }
}

WTEST_SUITE_RUNNER(WgeErrorUnexpected) {
    RUN_TEST_CASE(WgeErrorUnexpected, construct_unexpected);
    RUN_TEST_CASE(WgeErrorUnexpected, construct_unexpected_with_unexpected);
    RUN_TEST_CASE(WgeErrorUnexpected, assigne_other_unexpected);
    RUN_TEST_CASE(WgeErrorUnexpected, equality_of_unexpected);
}
