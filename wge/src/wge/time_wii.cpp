#include "wge/time.hpp"
#include "wge/types.hpp"

#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>

namespace wge {

namespace {
inline wge::u64 get_ticks() noexcept {
    const wge::u64 num_ticks = gettime();
    return num_ticks;
}
} // namespace

wge::u64 time_in_ms() noexcept {
    const auto num_ticks = get_ticks();
    return num_ticks / TB_TIMER_CLOCK;
}

} // namespace wge
