#ifndef WGE_WTEST_WTEST_HPP
#define WGE_WTEST_WTEST_HPP

extern "C" {
#include "detail/unity.h"
#include "detail/unity_internals.h"
#include "detail/unity_fixture.h"
}

#include "wge/types.hpp"

#define WTEST_SUITE(name) TEST_GROUP(name)
#define WTEST_SETUP(group) TEST_SETUP(group)
#define WTEST_TEAR_DOWN(group) TEST_TEAR_DOWN(group)
#define WTEST_CASE(group, name) TEST(group, name)

#define WTEST_SUITE_RUNNER(group) TEST_GROUP_RUNNER(group)

namespace wge {
namespace test {

template <typename T>
inline void assert_equal(const T& expected, const T& actual, unsigned int line, const char* msg) noexcept {
    UNITY_TEST_ASSERT((expected == actual), line, msg);
}

template <>
inline void assert_equal<wge::i8>(const wge::i8& expected, const wge::i8& actual, unsigned int line,
                                  const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_INT8(expected, actual, line, msg);
}

template <>
inline void assert_equal<wge::i16>(const wge::i16& expected, const wge::i16& actual, unsigned int line,
                                   const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_INT16(expected, actual, line, msg);
}

template <>
inline void assert_equal<wge::i32>(const wge::i32& expected, const wge::i32& actual, unsigned int line,
                                   const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_INT32(expected, actual, line, msg);
}

template <>
inline void assert_equal<wge::u8>(const wge::u8& expected, const wge::u8& actual, unsigned int line,
                                  const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_UINT8(expected, actual, line, msg);
}

template <>
inline void assert_equal<wge::u16>(const wge::u16& expected, const wge::u16& actual, unsigned int line,
                                   const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_UINT16(expected, actual, line, msg);
}

template <>
inline void assert_equal<wge::u32>(const wge::u32& expected, const wge::u32& actual, unsigned int line,
                                   const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_UINT32(expected, actual, line, msg);
}

inline void assert_equal(const char* expected, const char* actual, unsigned int line, const char* msg) noexcept {
    UNITY_TEST_ASSERT_EQUAL_STRING(expected, actual, line, msg);
}

template <typename T, wge::size_t>
struct asserter_equal_hex;

template <typename T>
struct asserter_equal_hex<T, 1> final {
    inline void operator()(const T& expected, const T& actual, unsigned int line, const char* msg) noexcept {
        UNITY_TEST_ASSERT_EQUAL_HEX8(expected, actual, line, msg);
    }
};

template <typename T>
struct asserter_equal_hex<T, 2> final {
    inline void operator()(const T& expected, const T& actual, unsigned int line, const char* msg) noexcept {
        UNITY_TEST_ASSERT_EQUAL_HEX16(expected, actual, line, msg);
    }
};

template <typename T>
struct asserter_equal_hex<T, 4> final {
    inline void operator()(const T& expected, const T& actual, unsigned int line, const char* msg) noexcept {
        UNITY_TEST_ASSERT_EQUAL_HEX32(expected, actual, line, msg);
    }
};

template <typename T>
inline void assert_equal_hex(const T& expected, const T& actual, unsigned int line, const char* msg) noexcept {
    asserter_equal_hex<T, sizeof(T)>()(expected, actual, line, msg);
}

}  // namespace test
}  // namespace wge

#define WTEST_ASSERT(condition) UNITY_TEST_ASSERT((condition), __LINE__, " Expression Evaluated To FALSE")
#define WTEST_ASSERT_TRUE(condition) UNITY_TEST_ASSERT((condition), __LINE__, " Expected TRUE Was FALSE")
#define WTEST_ASSERT_FALSE(condition) UNITY_TEST_ASSERT(!(condition), __LINE__, " Expected FALSE Was TRUE")

#define WTEST_ASSERT_EQUAL(expected, actual)                          \
    do {                                                              \
        wge::test::assert_equal(expected, actual, __LINE__, nullptr); \
    } while (0)

#define WTEST_ASSERT_EQUAL_HEX(expected, actual)                          \
    do {                                                                  \
        wge::test::assert_equal_hex(expected, actual, __LINE__, nullptr); \
    } while (0)

#define WTEST_ASSERT_EQUAL_MSG(expected, actual, msg)             \
    do {                                                          \
        wge::test::assert_equal(expected, actual, __LINE__, msg); \
    } while (0)

#define WTEST_ASSERT_EQUAL_HEX_MSG(expected, actual)                  \
    do {                                                              \
        wge::test::assert_equal_hex(expected, actual, __LINE__, msg); \
    }                                                                 \
    wh

#endif  // WGE_WTEST_WTEST_HPP
