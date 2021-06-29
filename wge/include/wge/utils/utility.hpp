#ifndef WOTH_WGE_UTILS_UTILITY_HPP
#define WOTH_WGE_UTILS_UTILITY_HPP

#include <type_traits>
#include <utility>

namespace wge {

template <class F>
class final_action {
public:
    static_assert(!std::is_reference<F>::value && !std::is_const<F>::value && !std::is_volatile<F>::value,
                  "Final_action should store its callable by value");

    explicit final_action(F f) noexcept : m_func(std::move(f)) {}

    final_action(final_action&& other) noexcept
        : m_func(std::move(other.m_func)), m_invoke(std::exchange(other.m_invoke, false)) {}

    final_action(const final_action&) = delete;
    final_action& operator=(const final_action&) = delete;
    final_action& operator=(final_action&&) = delete;

    ~final_action() noexcept {
        if (m_invoke) {
            m_func();
        }
    }

private:
    F m_func;
    bool m_invoke{true};
};

template <class F>
[[nodiscard]] final_action<typename std::remove_cv<typename std::remove_reference<F>::type>::type> finally(
    F&& f) noexcept {
    return final_action<typename std::remove_cv<typename std::remove_reference<F>::type>::type>(std::forward<F>(f));
}

namespace detail {
struct deferrer {
    template <typename F>
    final_action<typename std::remove_cv<typename std::remove_reference<F>::type>::type> operator<<(F&& f) {
        return finally(std::forward<F>(f));
    }
};
}  // namespace detail

}  // namespace wge

#define WGE_TOKEN_PASTE(x, y) x##y
#define WGE_TOKEN_PASTE2(x, y) WGE_TOKEN_PASTE(x, y)
#define wge_defer [[maybe_unused]] auto WGE_TOKEN_PASTE2(__deferred_lambda_call, __COUNTER__) = wge::detail::deferrer{} << [&]

#endif  // WOTH_WGE_UTILS_UTILITY_HPP
