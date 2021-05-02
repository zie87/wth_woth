#ifndef WOTH_WGE_STRING_VIEW_HPP
#define WOTH_WGE_STRING_VIEW_HPP

#if defined(__has_include) 
    #define WGE_HAS_INCLUDE(x) __has_include(x)
#else
    #define WGE_HAS_INCLUDE(x) 0
#endif

#if (WGE_HAS_INCLUDE(<string_view>) && (__cplusplus >= 201703L || defined(_LIBCPP_VERSION)))

    #include <string_view>

namespace wge {
template <typename Char>
using basic_string_view = std::basic_string_view<Char>;

using string_view = basic_string_view<char>;
}  // namespace wge

#elif WGE_HAS_INCLUDE("experimental/string_view") && __cplusplus >= 201402L

    #include <experimental/string_view>

namespace wge {
template <typename Char>
using basic_string_view = std::experimental::basic_string_view<Char>;

using string_view = basic_string_view<char>;
}  // namespace wge

#endif

#endif  // WOTH_WGE_STRING_VIEW_HPP
