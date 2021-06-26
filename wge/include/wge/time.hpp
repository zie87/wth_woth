#ifndef WOTH_WGE_TIME_HPP
#define WOTH_WGE_TIME_HPP

#include "wge/types.hpp"

#include <chrono>

namespace wge {

// get milliseconds since startup (build dependent)
wge::u64 time_in_ms() noexcept;

struct clock {
    using rep = wge::u64;
    using period = std::milli;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<clock>;

    constexpr static bool is_steady = true;

    static time_point now() noexcept { return time_point{duration{time_in_ms()}}; }
};

using time_point = clock::time_point;
using duration = clock::duration;

}  // namespace wge

#endif  // WOTH_WGE_TIME_HPP
