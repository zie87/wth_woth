#include "wge/thread/thread_psp.hpp"

#include <pspthreadman.h>

namespace wge {

namespace {

static int execute_thread_routine([[maybe_unused]] SceSize arg_size, void* funct_ptr) {
    auto* base_ptr = static_cast<thread::impl_base*>(funct_ptr);

    thread::shared_base_type local_ptr;
    local_ptr.swap(base_ptr->m_this_ptr);

    local_ptr->run();
    return 0;
}

} // namespace

void thread::start_thread(shared_base_type base_ptr) {
    base_ptr->m_this_ptr = base_ptr;
    constexpr int stack_size = 0x40000;
    m_id.m_thread = sceKernelCreateThread("", execute_thread_routine, 0x15, stack_size, PSP_THREAD_ATTR_USER, nullptr);

    const int error_code = sceKernelStartThread(m_id.m_thread, sizeof(shared_base_type), base_ptr.get());
    if (error_code < 0) {
        base_ptr->m_this_ptr.reset();
    }
}

void thread::join() {
    sceKernelTerminateDeleteThread(m_id.m_thread);
    m_id = id();
}

void thread::detach() {
    sceKernelDeleteThread(m_id.m_thread);
    m_id = id();
}

namespace detail {

void sleep_for_us(const std::chrono::microseconds sleep_duration) {
    sceKernelDelayThread(sleep_duration.count());
}

} // namespace detail

namespace this_thread {

thread::id get_id() noexcept {
    return thread::id(sceKernelGetThreadId());
}

} // namespace this_thread
} // namespace wge
