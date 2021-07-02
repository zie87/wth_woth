#ifndef WOTH_WGE_UTILS_TYPETRAITS_HPP
#define WOTH_WGE_UTILS_TYPETRAITS_HPP

#include <wge/utils/platform.hpp>

#include <type_traits>

#if WGE_CPP20_OR_GREATER

namespace wge {
using std::remove_cvref;
using std::remove_cvref_t;
}  // namespace wge

#else

namespace wge {
template <typename T>
struct remove_cvref {
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};
template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;
}  // namespace wge

#endif
#endif  // WOTH_WGE_UTILS_TYPETRAITS_HPP
