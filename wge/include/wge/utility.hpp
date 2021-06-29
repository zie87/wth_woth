#ifndef WOTH_WGE_UTILITY_HPP
#define WOTH_WGE_UTILITY_HPP

#include <utility>

namespace wge {

template <typename T, typename U>
[[nodiscard]] constexpr T narrow_cast(U&& u) noexcept {
    return static_cast<T>(std::forward<U>(u));
}

}  // namespace wge

#endif  // WOTH_WGE_UTILITY
