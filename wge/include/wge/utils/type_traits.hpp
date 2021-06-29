#ifndef WOTH_WGE_UTILS_TYPETRAITS_HPP
#define WOTH_WGE_UTILS_TYPETRAITS_HPP

#include <wge/utils/platform.hpp>

#include <type_traits>

namespace wge {

#if WGE_CPP20_OR_GREATER

using std::remove_cvref;

#else

template <typename T>
struct remove_cvref {
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

#endif

}  // namespace wge

#endif  // WOTH_WGE_UTILS_TYPETRAITS_HPP
