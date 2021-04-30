#include "wge/time.hpp"
#include "wge/types.hpp"

#include <chrono>
#include <ratio>

namespace wge {

namespace {

static const auto startup_time = std::chrono::system_clock::now();
} // namespace

wge::u64 time_in_ms() noexcept {
    using duration_t = std::chrono::duration<wge::u64, std::milli>;
    const auto current_time = std::chrono::system_clock::now();
    const auto time_since_start = std::chrono::duration_cast<duration_t>(current_time - startup_time);
    return time_since_start.count();
}
} // namespace wge
