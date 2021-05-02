#ifndef WOTH_WGE_STRING_VIEW_HPP
#define WOTH_WGE_STRING_VIEW_HPP

#if __cplusplus >= 201703L
    #include <string_view>

namespace wge {
template <typename Char>
using basic_string_view = std::basic_string_view<Char>;

using string_view = basic_string_view<char>;
}  // namespace wge

#elif __cplusplus >= 201402L

    #include <experimental/string_view>

namespace wge {
template <typename Char>
using basic_string_view = std::experimental::basic_string_view<Char>;

using string_view = basic_string_view<char>;
}  // namespace wge

#endif

#endif  // WOTH_WGE_STRING_VIEW_HPP
