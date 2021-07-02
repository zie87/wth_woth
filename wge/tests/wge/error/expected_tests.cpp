#include <wge/error/expected.hpp>

#include <wtest/wtest.hpp>

#include <string>

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

    {
        expected_type exp(42);
        WTEST_ASSERT_TRUE(exp.has_value());
        WTEST_ASSERT_EQUAL(42, exp.value());
    }

    {
        const expected_type exp(expected_type(16));
        WTEST_ASSERT_TRUE(exp.has_value());
        WTEST_ASSERT_EQUAL(16, exp.value());
    }
    {
        expected_type exp(expected_type::unexpected_type('B'));
        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL('B', exp.error());
    }
    {
        const expected_type::unexpected_type unexp('H');
        const expected_type exp(unexp);
        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL(unexp.value(), exp.error());
    }
    {
        const expected_type exp(123);
        const expected_type exp_cpy(exp);
        WTEST_ASSERT_TRUE(exp_cpy.has_value());
        WTEST_ASSERT_EQUAL(exp.value(), exp_cpy.value());
    }
    {
        expected_type exp(123);
        const expected_type exp_mov(std::move(exp));
        WTEST_ASSERT_TRUE(exp_mov.has_value());
        WTEST_ASSERT_EQUAL(123, exp_mov.value());
    }
}

WTEST_CASE(WgeErrorExpected, construct_with_expected_type) {
    using expected1_type = wge::expected<wge::i32, wge::i16>;
    using expected2_type = wge::expected<wge::u16, char>;

    {
        expected1_type exp(expected2_type::unexpected_type('B'));
        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL(static_cast<wge::i16>('B'), exp.error());
    }
    {
        const expected2_type::unexpected_type unexp('H');
        const expected1_type exp(unexp);
        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL(static_cast<wge::i16>(unexp.value()), exp.error());
    }
    {
        const expected2_type exp(123);
        const expected1_type exp_cpy(exp);
        WTEST_ASSERT_TRUE(exp_cpy.has_value());
        WTEST_ASSERT_EQUAL(static_cast<wge::i32>(exp.value()), exp_cpy.value());
    }
    {
        expected2_type exp(123);
        const expected1_type exp_mov(std::move(exp));
        WTEST_ASSERT_TRUE(exp_mov.has_value());
        WTEST_ASSERT_EQUAL(123, exp_mov.value());
    }
    {
        const expected2_type exp(expected2_type::unexpected_type('F'));
        const expected1_type exp_cpy(exp);
        WTEST_ASSERT_FALSE(exp_cpy.has_value());
        WTEST_ASSERT_EQUAL(static_cast<wge::i16>(exp.error()), exp_cpy.error());
    }
    {
        expected2_type exp(expected2_type::unexpected_type('M'));
        const expected1_type exp_mov(std::move(exp));
        WTEST_ASSERT_FALSE(exp_mov.has_value());
        WTEST_ASSERT_EQUAL(static_cast<wge::i16>('M'), exp_mov.error());
    }
}

WTEST_CASE(WgeErrorExpected, construct_with_initializer_list) {
    {
        using expected_type = wge::expected<std::string, wge::u8>;
        const expected_type exp(std::in_place, {'T', 'e', 's', 't'});
        WTEST_ASSERT_TRUE(exp.has_value());
        WTEST_ASSERT_EQUAL(std::string("Test"), exp.value());
    }

    {
        using expected_type = wge::expected<wge::u8, std::string>;
        const expected_type exp(wge::unexpect, {'E', 'r', 'r', 'o', 'r'});
        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL(std::string("Error"), exp.error());
    }
}

WTEST_CASE(WgeErrorExpected, value_or) {
    {
        using expected_type = wge::expected<wge::i32, char>;
        const expected_type exp(wge::unexpect, 'E');
        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL(42, exp.value_or(42));
    }
    {
        using expected_type = wge::expected<wge::i32, char>;
        const expected_type exp(17);
        WTEST_ASSERT_TRUE(exp.has_value());
        WTEST_ASSERT_EQUAL(17, exp.value_or(42));
    }
}

WTEST_CASE(WgeErrorExpected, assignment_operations) {
    {
        using expected_type   = wge::expected<wge::i32, char>;
        using unexpected_type = expected_type::unexpected_type;

        const expected_type exp(wge::unexpect, 'E');
        expected_type exp2(wge::unexpect, 'D');
        exp2 = exp;

        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL('E', exp.error());

        WTEST_ASSERT_TRUE(unexpected_type('D') != exp2);
        WTEST_ASSERT_TRUE(unexpected_type('E') == exp2);
    }
    {
        using expected_type = wge::expected<wge::i32, char>;
        using unexpected_type = expected_type::unexpected_type;

        const expected_type exp(wge::unexpect, 'E');
        expected_type exp2(32);
        exp2 = exp;

        WTEST_ASSERT_FALSE(exp.has_value());
        WTEST_ASSERT_EQUAL('E', exp.error());

        WTEST_ASSERT_TRUE(32 != exp2);
        WTEST_ASSERT_TRUE(unexpected_type('E') == exp2);
    }
    {
        using expected_type = wge::expected<wge::i32, char>;
        using unexpected_type = expected_type::unexpected_type;

        const expected_type exp(45);
        expected_type exp2(32);
        exp2 = exp;

        WTEST_ASSERT_TRUE(exp.has_value());
        WTEST_ASSERT_EQUAL(*exp, *exp2);
        WTEST_ASSERT_TRUE(32 != exp2);
        WTEST_ASSERT_TRUE(exp == exp2);
    }
}

WTEST_SUITE_RUNNER(WgeErrorExpected) {
    RUN_TEST_CASE(WgeErrorExpected, construct_default_expected);
    RUN_TEST_CASE(WgeErrorExpected, construct_with_same_expected_type);
    RUN_TEST_CASE(WgeErrorExpected, construct_with_expected_type);
    RUN_TEST_CASE(WgeErrorExpected, construct_with_initializer_list);
    RUN_TEST_CASE(WgeErrorExpected, assignment_operations);
    RUN_TEST_CASE(WgeErrorExpected, value_or);
}
