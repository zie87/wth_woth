#ifndef WGE_VIDEO_VRAMPTR_HPP
#define WGE_VIDEO_VRAMPTR_HPP

#include <wge/types.hpp>

#if defined(WOTH_PLATFORM_PSP)
namespace wge {
namespace video {

template <typename T>
struct vram_ptr final {
    using pointer_type = T*;

    vram_ptr(pointer_type ptr, bool vram) noexcept : m_buffer(ptr), m_is_vram(vram) {}
    ~vram_ptr() noexcept { reset(); }

    inline bool is_vram() const noexcept { return m_is_vram; }
    inline pointer_type get() const noexcept { return m_buffer; }
    inline pointer_type release() noexcept {
        auto* tmp = m_buffer;
        m_buffer = nullptr;
        return tmp;
    }

    inline bool is_valid() const noexcept { return m_buffer != nullptr; }
    explicit inline operator bool() const noexcept { return is_valid(); }

    vram_ptr(const vram_ptr&) = delete;
    vram_ptr(vram_ptr&&) = delete;
    vram_ptr& operator=(const vram_ptr&) = delete;
    vram_ptr& operator=(vram_ptr&&) = delete;

    inline void reset() noexcept {
        if (m_buffer != nullptr) {
            if (m_is_vram) {
                vfree(m_buffer);
            } else {
                free(m_buffer);
            }
            m_buffer = nullptr;
        }
    }

private:
    pointer_type m_buffer;
    bool m_is_vram;
};

template <typename T>
inline vram_ptr<T> make_vram_ptr(wge::size_t num_elm, bool is_vram) noexcept {
    constexpr auto type_size = sizeof(T);
    const auto size = num_elm * type_size;

    T* ptr = nullptr;
    if (is_vram) {
        ptr = reinterpret_cast<T*>(valloc(size));
    }

    if (ptr == nullptr) {
        ptr = reinterpret_cast<T*>(memalign(16, size));
        is_vram = false;
    }

    return {ptr, is_vram};
}

}  // namespace video
}  // namespace wge

#else

#include <wge/memory.hpp>

namespace wge {
namespace video {

template <typename T>
using vram_ptr = wge::unique_ptr<T[]>;

template <typename T>
inline vram_ptr<T> make_vram_ptr(wge::size_t num_elm, [[maybe_unused]] bool is_vram) noexcept {
    return std::make_unique<T[]>(num_elm);
}

}  // namespace video
}  // namespace wge

#endif

#endif  // WGE_VIDEO_VRAMPTR_HPP
