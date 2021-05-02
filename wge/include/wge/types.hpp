#ifndef WOTH_WGE_TYPES_HPP
#define WOTH_WGE_TYPES_HPP

#include <cstdint>
#include <cstddef>

namespace wge {
using byte_t = std::uint8_t;
using size_t = std::size_t;
using pixel_t = std::uint32_t;

using i8 = std::int8_t;
using u8 = std::uint8_t;

using i16 = std::int16_t;
using u16 = std::uint16_t;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i64 = std::int64_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

static_assert((sizeof(i8) == 1), "i8 size doesn't match expectation!");
static_assert((sizeof(u8) == 1), "u8 size doesn't match expectation!");
static_assert((sizeof(i16) == 2), "i16 size doesn't match expectation!");
static_assert((sizeof(u16) == 2), "u16 size doesn't match expectation!");
static_assert((sizeof(i32) == 4), "i32 size doesn't match expectation!");
static_assert((sizeof(u32) == 4), "u32 size doesn't match expectation!");
static_assert((sizeof(i64) == 8), "i64 size doesn't match expectation!");
static_assert((sizeof(u64) == 8), "z64 size doesn't match expectation!");

static_assert((sizeof(f32) == 4), "float_32 size doesn't match expectation!");
static_assert((sizeof(f64) == 8), "float_64 size doesn't match expectation!");

}  // namespace wge

#endif  // WOTH_WGE_TYPES_HPP
