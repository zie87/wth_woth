#ifndef WOTH_WGE_THREAD_MUTEXPSP_HPP
#define WOTH_WGE_THREAD_MUTEXPSP_HPP

namespace wge {

class mutex final {
public:
    using native_handle_type = int;

    mutex() noexcept;
    ~mutex() noexcept;

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    void lock();
    void unlock();

    // TODO: try_lock is missing
    inline native_handle_type native_handle() noexcept { return m_id; }

private:
    native_handle_type m_id = -1;
};

class recursive_mutex final {
    using thread_type = int;

public:
    using native_handle_type = int;

    recursive_mutex() noexcept;
    ~recursive_mutex() noexcept;

    recursive_mutex(const mutex&) = delete;
    recursive_mutex& operator=(const mutex&) = delete;

    void lock();
    void unlock();

    // TODO: try_lock is missing
    inline native_handle_type native_handle() noexcept { return m_id; }

private:
    thread_type m_thread_id = -1;
    native_handle_type m_id = -1;
    int m_recursion_count = 0;
};

}  // namespace wge

#include <mutex>

namespace wge {
using std::lock_guard;
}

#endif  // WOTH_WGE_THREAD_MUTEXPSP_HPP
