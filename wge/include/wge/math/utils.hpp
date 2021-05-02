#ifndef WGE_MATH_UTILS_HPP
#define WGE_MATH_UTILS_HPP

#include <cmath>
#include <type_traits>

namespace wge
{
  namespace math
  {
    template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
    inline T nearest_superior_power_of_2(T n)
    {
      auto value = std::pow(2, std::ceil(std::log(n) / std::log(2)));
      return static_cast<T>(value);
    }
  } // namespace math

} // namespace wge
#endif /* WGE_MATH_UTILS_HPP */
