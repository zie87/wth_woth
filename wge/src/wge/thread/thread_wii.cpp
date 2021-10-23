#include "wge/thread/thread_wii.hpp"

#include <chrono>
#include <ogcsys.h>

namespace wge {

namespace {

static void* execute_thread_routine(void* funct_ptr) {
    auto* base_ptr = static_cast<thread::impl_base*>(funct_ptr);

    thread::shared_base_type local_ptr;
    local_ptr.swap(base_ptr->m_this_ptr);

    local_ptr->run();
    return nullptr;
}

} // namespace

void thread::start_thread(shared_base_type base_ptr) {
    base_ptr->m_this_ptr = base_ptr;
    constexpr wge::u8 priority = 64u;
    constexpr wge::u32 stack_size = 0u; // use default stacksize

    const auto result =
        LWP_CreateThread(&(m_id.m_thread), execute_thread_routine, base_ptr.get(), nullptr, stack_size, priority);
    if (result != 0) {
        base_ptr->m_this_ptr.reset();
    }
}

void thread::join() {
    void* value_ptr;
    LWP_JoinThread(m_id.m_thread, &value_ptr);
    m_id = id();
}

/* void thread::detach() { */
/*     sceKernelDeleteThread(m_id.m_thread); */
/*     m_id = id(); */
/* } */

namespace detail {

void sleep_for_us(const std::chrono::microseconds sleep_duration) {
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(sleep_duration);
    std::chrono::nanoseconds nsec(sleep_duration - sec);

    struct timespec elapsed, tv;
    elapsed.tv_sec = sec.count();
    elapsed.tv_nsec = nsec.count();
    tv.tv_sec = elapsed.tv_sec;
    tv.tv_nsec = elapsed.tv_nsec;
    nanosleep(&tv, &elapsed);
}

} // namespace detail

namespace this_thread {

thread::id get_id() noexcept {
    return thread::id(LWP_GetSelf());
}

} // namespace this_thread
} // namespace wge
