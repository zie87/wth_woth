#ifndef WOTH_WGE_LOG_HPP
#define WOTH_WGE_LOG_HPP

#include "wge/log/log_core.hpp"
#include "wge/utils/macros.hpp"

// LEVEL [filename](line) MSG
#define WGE_LOG_MSG(lvl, fmt_pattern, ...)                                                                         \
    do {                                                                                                           \
        const auto pattern =                                                                                       \
            wge::format("{}: [{}:{}]({}) {}", wge::log::detail::level_to_str<lvl>(), WGE_SHORT_FILE, WGE_FUNCTION, \
                        WGE_STRINGIFY(WGE_LINE), wge::format(fmt_pattern, ##__VA_ARGS__));                         \
        wge::log::write_msg(pattern);                                                                              \
    } while (0)

// all possible error related warnings will be logged
#define WGE_LOG_FATAL(fmt_pattern, ...) WGE_LOG_MSG(wge::log::level::fatal, fmt_pattern, ##__VA_ARGS__)
#define WGE_LOG_ERROR(fmt_pattern, ...) WGE_LOG_MSG(wge::log::level::error, fmt_pattern, ##__VA_ARGS__)
#define WGE_LOG_WARN(fmt_pattern, ...) WGE_LOG_MSG(wge::log::level::warning, fmt_pattern, ##__VA_ARGS__)

// currently im not sure about the info level (on/off/switchable)
#define WGE_LOG_INFO(fmt_pattern, ...) WGE_LOG_MSG(wge::log::level::info, fmt_pattern, ##__VA_ARGS__)

// development related messages are only logged in debug build
#if defined(WOTH_DEBUG)
    #define WGE_LOG_DEBUG(fmt_pattern, ...) WGE_LOG_MSG(wge::log::level::debug, fmt_pattern, ##__VA_ARGS__)
    #define WGE_LOG_TRACE(fmt_pattern, ...) WGE_LOG_MSG(wge::log::level::trace, fmt_pattern, ##__VA_ARGS__)
#else
    #define WGE_LOG_DEBUG(fmt_pattern, ...) (void)0
    #define WGE_LOG_TRACE(fmt_pattern, ...) (void)0
#endif

#endif  // WOTH_WGE_LOG_HPP
