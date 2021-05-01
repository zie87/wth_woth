#ifndef WOTH_WGE_MEMORY_HPP
#define WOTH_WGE_MEMORY_HPP

#include <memory>

// TODO: define allocator!
namespace wge {
using std::shared_ptr;
using std::unique_ptr;
} // namespace wge

// TODO: scoped_ptr
namespace wge {
template <class T> using scoped_ptr = std::unique_ptr<T>;
}

#endif // WOTH_WGE_MEMORY_HPP
