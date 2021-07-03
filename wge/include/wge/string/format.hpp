#ifndef WOTH_WGE_STRING_FORMAT_HPP
#define WOTH_WGE_STRING_FORMAT_HPP

#include <wge/utils/platform.hpp>

#if WGE_CPP20_OR_GREATER && WGE_HAS_INCLUDE(<format>)

#include <format>

namespace wge {

using std::format;
using std::format_to;
using std::format_to_n;

using std::formatted_size;

using std::vformat;
using std::vformat_to;

}  // namespace wge

#else

#include <fmt/format.h>

namespace wge {

using fmt::format;
using fmt::format_to;
using fmt::format_to_n;

using fmt::formatted_size;

using fmt::vformat;
using fmt::vformat_to;

}  // namespace wge

#endif

#endif  // WOTH_WGE_STRING_FORMAT_HPP
