#ifndef WGE_MATH_UTILS_HPP
#define WGE_MATH_UTILS_HPP

#include "wge/types.hpp"

#include <cmath>
#include <type_traits>

namespace wge {
namespace math {

template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr inline T nearest_superior_power_of_2(T n) noexcept {
    auto value = std::pow(2, std::ceil(std::log(n) / std::log(2)));
    return static_cast<T>(value);
}

template <>
constexpr inline i32 nearest_superior_power_of_2<i32>(i32 width) noexcept {
    auto b = width;
    i32 n = 0;
    for (n = 0; b != 0; n++) b >>= 1;
    b = 1 << n;
    if (b == 2 * width) b >>= 1;
    return b;
}

}  // namespace math
}  // namespace wge

#endif /* WGE_MATH_UTILS_HPP */
