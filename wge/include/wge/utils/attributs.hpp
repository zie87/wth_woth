#ifndef WOTH_WGE_UTILS_ATTRIBUTS_HPP
#define WOTH_WGE_UTILS_ATTRIBUTS_HPP

#include <wge/utils/platform.hpp>

#if WGE_CPP20_OR_GREATER
#define WGE_LIKELY [[likely]]
#define WGE_UNLIKELY [[unlikely]]
#else
#define WGE_LIKELY
#define WGE_UNLIKELY
#endif

#endif  // WOTH_WGE_UTILS_ATTRIBUTS_HPP
