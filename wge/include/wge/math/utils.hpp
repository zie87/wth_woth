#ifndef WGE_MATH_UTILS_HPP
#define WGE_MATH_UTILS_HPP

#include <wge/types.hpp>

#include <type_traits>

namespace wge {
namespace math {

template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
constexpr inline T nearest_superior_power_of_2(T n) noexcept {
    T b = n;
    T cnt = 0;
    for (; b != 0; cnt++) {
        b >>= 1;
    }
    b = static_cast<T>(1 << cnt);
    if (b == 2 * n) {
        b >>= 1;
    }
    return b;
}

}  // namespace math

}  // namespace wge
#endif /* WGE_MATH_UTILS_HPP */
