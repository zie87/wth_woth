#include "wge/thread/mutex_wii.hpp"

#include <cassert>

#include <ogc/mutex.h>

namespace wge {

namespace {

mutex::native_handle_type create_mutex(bool use_recursive) noexcept {
    mutex::native_handle_type id = 0u;
    const auto result = LWP_MutexInit(&id, use_recursive);
    assert(result == 0);
    return id;
}

void destroy_mutex(const mutex::native_handle_type& id) noexcept {
    const auto result = LWP_MutexDestroy(id);
    assert(result == 0);
}

} // namespace

mutex::mutex() noexcept : m_id(create_mutex(false)) {}
mutex::~mutex() noexcept {
    destroy_mutex(m_id);
}

void mutex::lock() {
    const auto result = LWP_MutexLock(m_id);
    assert(result >= 0);
}

void mutex::unlock() {
    const auto result = LWP_MutexUnlock(m_id);
    assert(result >= 0);
}

recursive_mutex::recursive_mutex() noexcept : m_id(create_mutex(true)) {}
recursive_mutex::~recursive_mutex() noexcept {
    destroy_mutex(m_id);
}
void recursive_mutex::lock() {
    const auto result = LWP_MutexLock(m_id);
    assert(result >= 0);
}

void recursive_mutex::unlock() {
    const auto result = LWP_MutexUnlock(m_id);
    assert(result >= 0);
}
} // namespace wge
