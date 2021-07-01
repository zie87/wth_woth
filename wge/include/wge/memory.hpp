#ifndef WOTH_WGE_MEMORY_HPP
#define WOTH_WGE_MEMORY_HPP

#include <memory>

// TODO: define allocator!
namespace wge {
using std::shared_ptr;
using std::unique_ptr;
}  // namespace wge

#include <type_traits>

namespace wge {
template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
using owner_ptr = T;
}  // namespace wge

// TODO: scoped_ptr
namespace wge {
template <class T>
using scoped_ptr = std::unique_ptr<T>;
}  // namespace wge

namespace wge {
template <typename U>
static inline constexpr U* to_address(U* p) noexcept {
    return p;
}

template <typename Ptr, typename std::enable_if<!std::is_pointer<Ptr>::value, int>::type = 0>
static inline constexpr auto to_address(Ptr const& it) noexcept {
    return to_address(it.operator->());
}
}  // namespace wge

#endif  // WOTH_WGE_MEMORY_HPP
