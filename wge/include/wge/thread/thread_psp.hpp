#ifndef WOTH_WGE_THREAD_THREADPSP_HPP
#define WOTH_WGE_THREAD_THREADPSP_HPP

#include "wge/memory.hpp"

#include <pspthreadman.h>

#include <functional>

namespace wge {

class thread {
public:
    using native_handle_type = SceUID;

    class id {
        native_handle_type m_thread;

    public:
        static constexpr native_handle_type invalid_id = 0u;

        id() noexcept : m_thread(invalid_id) {}
        explicit id(native_handle_type handle) : m_thread(handle) {}

        inline bool is_valid() const noexcept { return m_thread > invalid_id; }

    private:
        friend class thread;
        friend bool operator==(id lhs, id rhs) noexcept;
    };

private:
    id m_id;

public:
    struct impl_base;
    typedef wge::shared_ptr<impl_base> shared_base_type;

    struct impl_base {
        shared_base_type m_this_ptr;

        inline virtual ~impl_base() = default;
        virtual void run() = 0;
    };

    template <typename Function>
    struct impl : public impl_base {
        Function function_obj;
        impl(Function&& func) : function_obj(std::forward<Function>(func)) {}
        void run() { function_obj(); }
    };

public:
    thread() noexcept = default;

    template <typename Function, typename... Args>
    explicit thread(Function&& func, Args&&... args) {
        start_thread(make_routine(std::bind(std::forward<Function>(func), std::forward<Args>(args)...)));
    }

    ~thread() noexcept {
        if (joinable()) std::terminate();
    }

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    thread(thread&& other) noexcept { swap(other); }

    thread& operator=(thread&& other) noexcept {
        if (joinable()) std::terminate();
        swap(other);
        return *this;
    }

    void swap(thread& other) noexcept {
        using std::swap;
        swap(m_id, other.m_id);
    }

    bool joinable() const noexcept { return m_id.is_valid(); }

    void join();
    void detach();

    id get_id() const noexcept { return m_id; }

    native_handle_type native_handle() const noexcept { return m_id.m_thread; }

    static constexpr unsigned int hardware_concurrency() noexcept { return 1u; }

private:
    void start_thread(shared_base_type base_ptr);

    template <typename function>
    auto make_routine(function&& func) -> shared_ptr<impl<function>> {
        return std::make_shared<impl<function>>(std::forward<function>(func));
    }
};

inline bool operator==(thread::id lhs, thread::id rhs) noexcept {
    if (!(lhs.is_valid()) && !(rhs.is_valid())) {
        return true;
    }

    return lhs.m_thread == rhs.m_thread;
}

inline bool operator!=(thread::id lhs, thread::id rhs) noexcept { return !(lhs == rhs); }

inline void swap(thread& lhs, thread& rhs) noexcept { lhs.swap(rhs); }

}  // namespace wge

#include <chrono>

namespace wge {

namespace detail {
void sleep_for_us(const std::chrono::microseconds sleep_duration);
}

namespace this_thread {

wge::thread::id get_id() noexcept;

template <class Rep, class Period>
void sleep_for(const std::chrono::duration<Rep, Period>& sleep_duration) {
    const std::chrono::microseconds time(sleep_duration);
    detail::sleep_for_us(time);
}

}  // namespace this_thread

}  // namespace wge

#endif  // WOTH_WGE_THREAD_THREADPSP_HPP
