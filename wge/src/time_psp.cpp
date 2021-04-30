#include "wge/time.hpp"
#include "wge/types.hpp"

#include <psptypes.h>
#include <psprtc.h>

namespace wge {

namespace {
inline wge::u64 get_ticks() noexcept {
    wge::u64 num_ticks = 0u;
    sceRtcGetCurrentTick(&num_ticks);
    return num_ticks;
}

static const auto startup_ticks = get_ticks();
} // namespace

wge::u64 time_in_ms() noexcept {
    static const auto tick_resolution = sceRtcGetTickResolution();
    const auto num_ticks = get_ticks() - startup_ticks;
    return (num_ticks * 1000) / tick_resolution;
}

} // namespace wge
