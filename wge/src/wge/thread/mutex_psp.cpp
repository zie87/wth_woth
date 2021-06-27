#include "wge/thread/mutex_psp.hpp"

#include <pspthreadman.h>

#include <cassert>

namespace wge {

mutex::mutex() noexcept : m_id(sceKernelCreateSema("Unnamed", 0, 1, 1, 0)) {}
mutex::~mutex() noexcept { sceKernelDeleteSema(m_id); }

void mutex::lock() {
    [[maybe_unused]] const auto result = sceKernelWaitSema(m_id, 1, 0);
    // FIXME: ERROR HANDLING
    assert(result >= 0);
}

void mutex::unlock() {
    [[maybe_unused]] const auto result = sceKernelSignalSema(m_id, 1);
    // FIXME: ERROR HANDLING
    assert(result >= 0);
}

recursive_mutex::recursive_mutex() noexcept : m_id(sceKernelCreateSema("Unnamed", 0, 1, 1, 0)) {}
recursive_mutex::~recursive_mutex() noexcept { sceKernelDeleteSema(m_id); }

void recursive_mutex::lock() {
    const auto thread_id = sceKernelGetThreadId();

    if (m_thread_id == thread_id) {
        ++m_recursion_count;
        return;
    }

    [[maybe_unused]] const auto result = sceKernelWaitSema(m_id, 1, 0);
    // FIXME: ERROR HANDLING
    assert(result >= 0);
    m_thread_id = thread_id;
    m_recursion_count = 1;
}

void recursive_mutex::unlock() {
    --m_recursion_count;
    // FIXME: ERROR HANDLING
    assert(m_recursion_count >= 0);
    if (m_recursion_count == 0) {
        [[maybe_unused]] const auto result = sceKernelSignalSema(m_id, 1);
        assert(result >= 0);
    }
}
}  // namespace wge
