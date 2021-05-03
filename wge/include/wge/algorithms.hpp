#ifndef WOTH_WGE_ALGORTHMS_HPP
#define WOTH_WGE_ALGORTHMS_HPP

#include <algorithm>

#include <random>
#include <chrono>

namespace wge {

template <class RandomIt>
inline void shuffle(RandomIt first, RandomIt last, unsigned seed) {
    std::shuffle(first, last, std::default_random_engine(seed));
}

template <class RandomIt>
inline void shuffle(RandomIt first, RandomIt last) {
    // TODO: change to wge internal clock!
    const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(first, last, seed);
}
}  // namespace wge

#endif  // WOTH_WGE_ALGORTHMS_HPP
