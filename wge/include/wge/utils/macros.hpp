#ifndef WOTH_WGE_UTILS_MACROS_HPP
#define WOTH_WGE_UTILS_MACROS_HPP

// numbers as string
#define WGE_AS_STR(x) #x
#define WGE_STRINGIFY(x) WGE_AS_STR(x)

namespace wge {
namespace detail {
// based on https://blog.galowicz.de/2016/02/20/short_file_macro/
using cstr = const char*;
static constexpr cstr past_last_slash(cstr str, cstr last_slash) {
    return *str == '\0'  ? last_slash
           : *str == '/' ? past_last_slash(str + 1, str + 1)
                         : past_last_slash(str + 1, last_slash);
}

static constexpr cstr past_last_slash(cstr str) { return past_last_slash(str, str); }
}  // namespace detail
}  // namespace wge

#define WGE_LINE __LINE__
#define WGE_FILE __FILE__
#define WGE_SHORT_FILE wge::detail::past_last_slash(WGE_FILE)
#define WGE_FUNCTION __FUNCTION__

#endif  // WOTH_WGE_UTILS_MACROS_HPP
