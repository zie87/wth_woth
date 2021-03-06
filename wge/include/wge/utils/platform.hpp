#ifndef WOTH_WGE_UTILS_PLATFORM_HPP
#define WOTH_WGE_UTILS_PLATFORM_HPP

#if defined(__has_include)
#define WGE_HAS_INCLUDE(x) __has_include(x)
#else
#define WGE_HAS_INCLUDE(x) 0
#endif

#define WGE_CPP98_OR_GREATER (__cplusplus >= 199711L)
#define WGE_CPP11_OR_GREATER (__cplusplus >= 201103L)
#define WGE_CPP14_OR_GREATER (__cplusplus >= 201402L)
#define WGE_CPP17_OR_GREATER (__cplusplus >= 201703L)
#define WGE_CPP20_OR_GREATER (__cplusplus >= 202000L)

#endif  // WOTH_WGE_UTILS_PLATFORM_HPP
