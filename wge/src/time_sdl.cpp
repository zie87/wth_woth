#include "wge/time.hpp"
#include "wge/types.hpp"

#include <SDL_timer.h>

namespace wge {

namespace {
inline wge::u64 get_ticks() noexcept {
    return SDL_GetTicks();
}
} // namespace

wge::u64 time_in_ms() noexcept {
    return get_ticks();
}

} // namespace wge
