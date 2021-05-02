#ifndef WOTH_WGE_LOG_LOGCORE_HPP
#define WOTH_WGE_LOG_LOGCORE_HPP

#include "wge/string/format.hpp"
#include "wge/string/view.hpp"

namespace wge {
namespace log {

enum class level { fatal, error, warning, info, debug, trace };

void write_msg(wge::string_view msg) noexcept;

namespace detail {

template <wge::log::level>
constexpr wge::string_view level_to_str();

template <>
inline constexpr wge::string_view level_to_str<wge::log::level::fatal>() {
    return "[FATAL]  ";
}

template <>
inline constexpr wge::string_view level_to_str<wge::log::level::error>() {
    return "[ERROR]  ";
}

template <>
inline constexpr wge::string_view level_to_str<wge::log::level::warning>() {
    return "[WARNING]";
}

template <>
inline constexpr wge::string_view level_to_str<wge::log::level::info>() {
    return "[INFO]   ";
}

template <>
inline constexpr wge::string_view level_to_str<wge::log::level::debug>() {
    return "[DEBUG]  ";
}

template <>
inline constexpr wge::string_view level_to_str<wge::log::level::trace>() {
    return "[TRACE]  ";
}

}  // namespace detail

}  // namespace log
}  // namespace wge

#endif  // WOTH_WGE_LOG_LOGCORE_HPP
