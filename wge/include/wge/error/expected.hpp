#ifndef WOTH_WGE_ERROR_EXPECTED_HPP
#define WOTH_WGE_ERROR_EXPECTED_HPP

#include <wge/error/unexpected.hpp>
#include <wge/error/storage.hpp>
#include <wge/debug/assert.hpp>

#include <initializer_list>
#include <type_traits>

namespace wge {

namespace detail {

template <class T, class E>
class expected;

template <class T, class E, class U>
using expected_enable_forward_value =
    std::enable_if_t<std::is_constructible_v<T, U&&> && !std::is_same_v<wge::remove_cvref_t<U>, std::in_place_t> &&
                     !std::is_same_v<expected<T, E>, wge::remove_cvref_t<U>> &&
                     !std::is_same_v<unexpected<E>, wge::remove_cvref_t<U>>>;

template <class V, class E, class V2, class E2, class V2R, class E2R>
using expected_enable_from_other = std::enable_if_t<
    std::is_constructible_v<V, V2R> && std::is_constructible_v<E, E2R> &&
    !std::is_constructible_v<V, expected<V2, E2>&> && !std::is_constructible_v<V, expected<V2, E2>&&> &&
    !std::is_constructible_v<V, const expected<V2, E2>&> && !std::is_constructible_v<V, const expected<V2, E2>&&> &&
    !std::is_convertible_v<expected<V2, E2>&, V> && !std::is_convertible_v<expected<V2, E2>&&, V> &&
    !std::is_convertible_v<const expected<V2, E2>&, V> && !std::is_convertible_v<const expected<V2, E2>&&, V>>;

inline bool error_text(char const* /*text*/) { return true; }

}  // namespace detail

template <typename Error>
struct error_traits {
    static void rethrow(Error const& /*e*/) { wge_ensures(false && detail::error_text("bad expected access;")); }
};

template <class T, class E>
class expected {
    using storage_type = detail::storage<T, E, std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E>,
                                         std::is_move_constructible_v<T> && std::is_move_constructible_v<E>>;

    using error_traits_type = error_traits<E>;

public:
    using value_type      = T;
    using error_type      = E;
    using unexpected_type = unexpected<E>;

    template <class U>
    using rebind = expected<U, error_type>;

    template <bool B = std::is_default_constructible_v<value_type>, typename std::enable_if_t<B, int> = 0>
    constexpr expected() : m_storage(true) {
        m_storage.construct_value(value_type());
    }

    constexpr expected(const expected&)     = default;
    constexpr expected(expected&&) noexcept = default;

    template <class U, class G,
              detail::expected_enable_from_other<value_type, error_type, U, G, const U&, const G&>* = nullptr,
              std::enable_if_t<!(std::is_convertible_v<const U&, value_type> &&
                                 std::is_convertible_v<const G&, error_type>)>* = nullptr> /* explicit condition*/
    constexpr explicit expected(const expected<U, G>& other) : m_storage(other.has_value()) {
        if (has_value()) {
            m_storage.construct_value(other.value());
        } else {
            m_storage.construct_error(other.error());
        }
    }

    template <class U, class G,
              detail::expected_enable_from_other<value_type, error_type, U, G, const U&, const G&>* = nullptr,
              std::enable_if_t<std::is_convertible_v<const U&, value_type> &&
                               std::is_convertible_v<const G&, error_type>>* = nullptr> /* explicit condition*/
    constexpr /*non explicit*/ expected(const expected<U, G>& other) : m_storage(other.has_value()) {
        if (has_value()) {
            m_storage.construct_value(other.value());
        } else {
            m_storage.construct_error(other.error());
        }
    }

    template <class U, class G, detail::expected_enable_from_other<value_type, error_type, U, G, U, G>* = nullptr,
              std::enable_if_t<!(std::is_convertible_v<const U&, value_type> &&
                                 std::is_convertible_v<const G&, error_type>)>* = nullptr> /* explicit condition*/
    constexpr explicit expected(expected<U, G>&& other) : m_storage(other.has_value()) {
        if (has_value()) {
            m_storage.construct_value(std::move(other.value()));
        } else {
            m_storage.construct_error(std::move(other.error()));
        }
    }

    template <class U, class G, detail::expected_enable_from_other<value_type, error_type, U, G, U, G>* = nullptr,
              std::enable_if_t<std::is_convertible_v<const U&, value_type> &&
                               std::is_convertible_v<const G&, error_type>>* = nullptr> /* explicit condition*/
    constexpr /*non explicit*/ expected(expected<U, G>&& other) : m_storage(other.has_value()) {
        if (has_value()) {
            m_storage.construct_value(std::move(other.value()));
        } else {
            m_storage.construct_error(std::move(other.error()));
        }
    }

    template <class U = value_type, detail::expected_enable_forward_value<value_type, error_type, U>* = nullptr,
              std::enable_if_t<!std::is_convertible_v<U&&, value_type>>* = nullptr> /* explicit condition*/
    constexpr explicit expected(U&& v) : expected(std::in_place, std::forward<U>(v)) {}

    template <class U = value_type, detail::expected_enable_forward_value<value_type, error_type, U>* = nullptr,
              std::enable_if_t<std::is_convertible_v<U&&, value_type>>* = nullptr> /* explicit condition*/
    constexpr /*non explicit*/ expected(U&& v) : expected(std::in_place, std::forward<U>(v)) {}

    template <class G                                                                        = error_type,
              std::enable_if_t<std::is_constructible_v<error_type, const G&> &&
                               !std::is_convertible_v<const G&, E> /* explicit condition*/>* = nullptr>
    constexpr explicit expected(const unexpected<G>& e) : m_storage(false) {
        m_storage.construct_error(e.value());
    }

    template <class G                                                                       = error_type,
              std::enable_if_t<std::is_constructible_v<error_type, const G&> &&
                               std::is_convertible_v<const G&, E> /* explicit condition*/>* = nullptr>
    constexpr /*non explicit*/ expected(const unexpected<G>& e) : m_storage(false) {
        m_storage.construct_error(e.value());
    }

    template <class G = error_type, std::enable_if_t<std::is_constructible_v<error_type, G&&> &&
                                                     !std::is_convertible_v<G&&, E> /* explicit condition*/>* = nullptr>
    constexpr explicit expected(unexpected<G>&& e) : m_storage(false) {
        m_storage.construct_error(std::move(e.value()));
    }

    template <class G = error_type, std::enable_if_t<std::is_constructible_v<error_type, G&&> &&
                                                     std::is_convertible_v<G&&, E> /* explicit condition*/>* = nullptr>
    constexpr /*non explicit*/ expected(unexpected<G>&& e) : m_storage(false) {
        m_storage.construct_error(std::move(e.value()));
    }

    template <class... Args, std::enable_if_t<std::is_constructible<value_type, Args&&...>::value>* = nullptr>
    constexpr explicit expected(std::in_place_t, Args&&... args) : m_storage(true) {
        m_storage.emplace_value(std::forward<Args>(args)...);
    }

    template <
        class U, class... Args,
        std::enable_if_t<std::is_constructible<value_type, std::initializer_list<U>, Args&&...>::value>* = nullptr>
    constexpr explicit expected(std::in_place_t, std::initializer_list<U> init_list, Args&&... args) : m_storage(true) {
        m_storage.emplace_value(init_list, std::forward<Args>(args)...);
    }

    template <class... Args, std::enable_if_t<std::is_constructible<error_type, Args&&...>::value>* = nullptr>
    constexpr explicit expected(unexpect_t, Args&&... args) : m_storage(false) {
        m_storage.emplace_error(std::forward<Args>(args)...);
    }

    template <
        class U, class... Args,
        std::enable_if_t<std::is_constructible<error_type, std::initializer_list<U>, Args&&...>::value>* = nullptr>
    constexpr explicit expected(unexpect_t, std::initializer_list<U> init_list, Args&&... args) : m_storage(false) {
        m_storage.emplace_error(init_list, std::forward<Args>(args)...);
    }

    ~expected() {
        if (has_value()) {
            m_storage.destruct_value();
        } else {
            m_storage.destruct_error();
        }
    }

    expected& operator=(const expected& other) {
        expected(other).swap(*this);
        return *this;
    }

    expected& operator=(expected&& other) noexcept(
        std::is_nothrow_constructible_v<T>&& std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_constructible_v<E>&&
            std::is_nothrow_move_assignable_v<E>) {
        expected(std::move(other)).swap(*this);
        return *this;
    }

    template <
        class U                                                             = T,
        std::enable_if_t<!std::is_same_v<expected<value_type, error_type>, wge::remove_cvref_t<U>> &&
                         !std::conjunction_v<std::is_scalar<value_type>, std::is_same<value_type, std::decay_t<U>>> &&
                         std::is_constructible_v<value_type, U> && std::is_assignable_v<value_type&, U> &&
                         std::is_nothrow_move_constructible_v<error_type>>* = nullptr>
    expected& operator=(U&& v) {
        expected(std::forward<U>(v)).swap(*this);
        return *this;
    }

    template <class G = error_type, std::enable_if_t<std::is_copy_constructible_v<error_type> &&
                                                     std::is_copy_assignable_v<error_type>>* = nullptr>
    expected& operator=(const unexpected<G>& e) {
        expected(unexpect, e.value()).swap(*this);
        return *this;
    }

    template <class G = error_type, std::enable_if_t<std::is_move_constructible_v<error_type> &&
                                                     std::is_move_assignable_v<error_type>>* = nullptr>
    expected& operator=(unexpected<G>&& e) {
        expected(unexpect, std::move(e.value())).swap(*this);
        return *this;
    }

    template <class... Args, std::enable_if_t<std::is_nothrow_constructible_v<T, Args&&...>>* = nullptr>
    value_type& emplace(Args&&... args) {
        expected(std::in_place, std::forward<Args>(args)...).swap(*this);
        return value();
    }

    template <class U, class... Args,
              std::enable_if_t<std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args&&...>>* = nullptr>
    value_type& emplace(std::initializer_list<U> init_list, Args&&... args) {
        expected(std::in_place, init_list, std::forward<Args>(args)...).swap(*this);
        return value();
    }

    template <typename U = T, typename G = E>
    auto swap(expected& other) noexcept(
        std::is_nothrow_move_constructible_v<value_type>&& std::is_nothrow_swappable_v<value_type>&&
            std::is_nothrow_move_constructible_v<error_type>&& std::is_nothrow_swappable_v<error_type>)
        -> std::enable_if_t<(std::is_swappable_v<U> && std::is_swappable_v<G> &&
                             (std::is_move_constructible_v<U> || std::is_move_constructible_v<G>)),
                            void> {
        using std::swap;

        if (bool(*this) && bool(other)) {
            swap(m_storage.value(), other.m_storage.value());
            return;
        }

        if (!bool(*this) && !bool(other)) {
            swap(m_storage.error(), other.m_storage.error());
            return;
        }

        if (!bool(*this) && bool(other)) {
            other.swap(*this);
            return;
        }

        error_type t(std::move(other.error()));
        other.m_storage.destruct_error();
        other.m_storage.construct_value(std::move(m_storage.value()));
        m_storage.destruct_value();
        m_storage.construct_error(std::move(t));
        const bool has_value       = m_storage.has_value();
        const bool other_has_value = other.has_value();
        other.m_storage.set_has_value(has_value);
        m_storage.set_has_value(other_has_value);
    }

    constexpr auto operator->() const -> const value_type* {
        wge_expects(has_value());
        return m_storage.value_ptr();
    }
    constexpr auto operator->() -> value_type* {
        wge_expects(has_value());
        return m_storage.value_ptr();
    }

    constexpr auto operator*() const& -> const value_type& {
        wge_expects(has_value());
        return m_storage.value();
    }
    constexpr auto operator*() & -> value_type& {
        wge_expects(has_value());
        return m_storage.value();
    }
    constexpr auto operator*() const&& -> const value_type&& {
        wge_expects(has_value());
        return std::move(m_storage.value());
    }
    constexpr auto operator*() && -> value_type&& {
        wge_expects(has_value());
        return std::move(m_storage.value());
    }

    constexpr explicit operator bool() const noexcept { return has_value(); }
    constexpr bool has_value() const noexcept { return m_storage.has_value(); }

    constexpr auto value() const& -> const value_type& {
        return has_value() ? m_storage.value() : (error_traits_type::rethrow(m_storage.error()), m_storage.value());
    }

    constexpr auto value() & -> value_type& {
        return has_value() ? m_storage.value() : (error_traits_type::rethrow(m_storage.error()), m_storage.value());
    }

    constexpr auto value() const&& -> const value_type&& {
        return std::move(has_value() ? m_storage.value()
                                     : (error_traits_type::rethrow(m_storage.error()), m_storage.value()));
    }

    constexpr auto value() && -> value_type&& {
        return std::move(has_value() ? m_storage.value()
                                     : (error_traits_type::rethrow(m_storage.error()), m_storage.value()));
    }

    constexpr auto error() const& -> const error_type& {
        wge_expects(!has_value());
        return m_storage.error();
    }
    constexpr auto error() & -> error_type& {
        wge_expects(!has_value());
        return m_storage.error();
    }
    constexpr auto error() const&& -> const error_type&& {
        wge_expects(!has_value());
        return std::move(m_storage.error());
    }
    constexpr auto error() && -> error_type&& {
        wge_expects(!has_value());
        return std::move(m_storage.error());
    }

    template <class U, std::enable_if_t<std::is_copy_constructible_v<value_type> &&
                                        std::is_convertible_v<U&&, value_type>>* = nullptr>
    constexpr auto value_or(U&& v) const& -> value_type {
        return has_value() ? m_storage.value() : std::forward<U>(v);
    }

    template <class U, std::enable_if_t<std::is_move_constructible_v<value_type> &&
                                        std::is_convertible_v<U&&, value_type>>* = nullptr>
    constexpr auto value_or(U&& v) && -> value_type {
        return has_value() ? std::move(m_storage.value()) : std::forward<U>(v);
    }

private:
    storage_type m_storage;
};

template <class T1, class E1, class T2, class E2>
constexpr bool operator==(const expected<T1, E1>& x, const expected<T2, E2>& y) {
    return bool(x) != bool(y) ? false : bool(x) == false ? x.error() == y.error() : *x == *y;
}
template <class T1, class E1, class T2, class E2>
constexpr bool operator!=(const expected<T1, E1>& x, const expected<T2, E2>& y) {
    return !(x == y);
}

template <class T1, class E1, class T2>
constexpr bool operator==(const expected<T1, E1>& lhs, const T2& rhs) {
    return bool(lhs) ? (*lhs == rhs) : false;
}
template <class T1, class E1, class T2>
constexpr bool operator==(const T2& lhs, const expected<T1, E1>& rhs) {
    return (rhs == lhs);
}
template <class T1, class E1, class T2>
constexpr bool operator!=(const expected<T1, E1>& lhs, const T2& rhs) {
    return !(lhs == rhs);
}
template <class T1, class E1, class T2>
constexpr bool operator!=(const T2& lhs, const expected<T1, E1>& rhs) {
    return !(rhs == lhs);
}

template <class T1, class E1, class E2>
constexpr bool operator==(const expected<T1, E1>& lhs, const unexpected<E2>& rhs) {
    return bool(lhs) ? false : (lhs.error() == rhs.value());
}
template <class T1, class E1, class E2>
constexpr bool operator==(const unexpected<E2>& lhs, const expected<T1, E1>& rhs) {
    return (rhs == lhs);
}
template <class T1, class E1, class E2>
constexpr bool operator!=(const expected<T1, E1>& lhs, const unexpected<E2>& rhs) {
    return !(lhs == rhs);
}
template <class T1, class E1, class E2>
constexpr bool operator!=(const unexpected<E2>& lhs, const expected<T1, E1>& rhs) {
    return !(rhs == lhs);
}

template <class T, class E,
          std::enable_if_t<(std::is_void_v<T> || std::is_move_constructible_v<T>)&&std::is_swappable_v<T> &&
                           std::is_move_constructible_v<E> && std::is_swappable_v<E>>* = nullptr>
void swap(expected<T, E>& x, expected<T, E> y) noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

}  // namespace wge

#endif  // WOTH_WGE_ERROR_EXPECTED_HPP
