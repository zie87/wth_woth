#include <wge/debug/log_core.hpp>
#include <wge/string/view.hpp>

#include <fstream>
#include <iostream>

namespace wge {
namespace log {
namespace detail {

inline void log_to_stdout(wge::string_view msg) noexcept {
    std::cout << msg << '\n';
}

inline void log_to_file(wge::string_view msg) noexcept {
    std::ofstream file("wge_log.txt", std::ios_base::app);
    file << msg << '\n';
}

} // namespace detail
} // namespace log
} // namespace wge

namespace wge {
namespace log {

void write_msg(wge::string_view msg) noexcept {
#if defined(WOTH_PLATFORM_PSP)
    detail::log_to_file(msg);
#else
    detail::log_to_stdout(msg);
#endif
}

} // namespace log
} // namespace wge
