#ifndef WOTH_WGE_UTILS_WARNINGS_HPP
#define WOTH_WGE_UTILS_WARNINGS_HPP

// idea is based on https://www.fluentcpp.com/2019/08/30/how-to-disable-a-warning-in-cpp/

// clang-format off
#if defined(_MSC_VER)
#define WGE_DISABLE_WARNING_PUSH            __pragma(warning(push))
#define WGE_DISABLE_WARNING_POP             __pragma(warning(pop))
#define WGE_DISABLE_WARNING(warningNumber)  __pragma(warning(disable : warningNumber))

#define WGE_DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER   WGE_DISABLE_WARNING(4100)
#define WGE_DISABLE_WARNING_UNREFERENCED_FUNCTION           WGE_DISABLE_WARNING(4505)
#define WGE_DISABLE_WARNING_CAST_ALIGN                      // unkown

#elif defined(__GNUC__) || defined(__clang__)
#define WGE_DO_PRAGMA(X) _Pragma(#X)
#define WGE_DISABLE_WARNING_PUSH         WGE_DO_PRAGMA(GCC diagnostic push)
#define WGE_DISABLE_WARNING_POP          WGE_DO_PRAGMA(GCC diagnostic pop)
#define WGE_DISABLE_WARNING(warningName) WGE_DO_PRAGMA(GCC diagnostic ignored #warningName)

#define WGE_DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER WGE_DISABLE_WARNING(-Wunused-parameter)
#define WGE_DISABLE_WARNING_UNREFERENCED_FUNCTION         WGE_DISABLE_WARNING(-Wunused-function)
#define WGE_DISABLE_WARNING_CAST_ALIGN                    WGE_DISABLE_WARNING(-Wcast-align)

#else
#define WGE_DISABLE_WARNING_PUSH
#define WGE_DISABLE_WARNING_POP

#define WGE_DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define WGE_DISABLE_WARNING_UNREFERENCED_FUNCTION
#define WGE_DISABLE_WARNING_CAST_ALIGN 

#endif

// clang-format on
#endif  // WOTH_WGE_UTILS_WARNINGS_HPP
