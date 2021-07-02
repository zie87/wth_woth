#ifndef WOTH_WGE_ERROR_EXPECTED_HPP
#define WOTH_WGE_ERROR_EXPECTED_HPP

#include <wge/error/unexpected.hpp>
#include <wge/error/storage.hpp>

namespace wge {

template <class T, class E>
class expected {
    using storage_type = detail::storage<T, E, std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E>,
                                         std::is_move_constructible_v<T> && std::is_move_constructible_v<E> >;

public:
    using value_type      = T;
    using error_type      = E;
    using unexpected_type = unexpected<E>;

    template <class U>
    using rebind = expected<U, error_type>;

    // �.�.4.1, constructors

    template <bool B = std::is_default_constructible_v<value_type>, typename std::enable_if_t<B, int> = 0>
    constexpr expected() : m_storage(true) {
        m_storage.construct_value(value_type());
    }

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) noexcept = default;


    // template<class U, class G>
    //     explicit(see below) constexpr expected(const expected<U, G>&);
    // template<class U, class G>
    //     explicit(see below) constexpr expected(expected<U, G>&&);

    // template<class U = T>
    //     explicit(see below) constexpr expected(U&& v);

    // template<class G = E>
    //     constexpr expected(const unexpected<G>&);
    // template<class G = E>
    //     constexpr expected(unexpected<G>&&);

    // template<class... Args>
    //     constexpr explicit expected(in_place_t, Args&&...);
    // template<class U, class... Args>
    //     constexpr explicit expected(in_place_t, initializer_list<U>, Args&&...);
    // template<class... Args>
    //     constexpr explicit expected(unexpect_t, Args&&...);
    // template<class U, class... Args>
    //     constexpr explicit expected(unexpect_t, initializer_list<U>, Args&&...);

    // �.�.4.2, destructor
    ~expected() {
        if (has_value()) {
            m_storage.destruct_value();
        } else {
            m_storage.destruct_error();
        }
    }

    // �.�.4.3, assignment
    // expected& operator=(const expected&);
    // expected& operator=(expected&&) noexcept(see below);
    // template<class U = T> expected& operator=(U&&);
    // template<class G = E>
    //    expected& operator=(const unexpected<G>&);
    // template<class G = E>
    //    expected& operator=(unexpected<G>&&);

    // �.�.4.4, modifiers

    // template<class... Args>
    //     T& emplace(Args&&...);
    // template<class U, class... Args>
    //     T& emplace(initializer_list<U>, Args&&...);

    // �.�.4.5, swap
    // void swap(expected&) noexcept(see below);

    // �.�.4.6, observers
    // constexpr const T* operator->() const;
    // constexpr T* operator->();
    // constexpr const T& operator*() const&;
    // constexpr T& operator*() &;
    // constexpr const T&& operator*() const&&;
    // constexpr T&& operator*() &&;

    constexpr explicit operator bool() const noexcept { return has_value(); }
    constexpr bool has_value() const noexcept { return m_storage.has_value(); }

    // constexpr const T& value() const&;
    // constexpr T& value() &;
    // constexpr const T&& value() const&&;
    // constexpr T&& value() &&;

    // constexpr const E& error() const&;
    // constexpr E& error() &;
    // constexpr const E&& error() const&&;
    // constexpr E&& error() &&;
    // template<class U>
    //     constexpr T value_or(U&&) const&;
    // template<class U>
    //     constexpr T value_or(U&&) &&;

    // �.�.4.7, Expected equality operators
    // template<class T1, class E1, class T2, class E2>
    //    friend constexpr bool operator==(const expected<T1, E1>& x, const expected<T2, E2>& y);
    // template<class T1, class E1, class T2, class E2>
    //    friend constexpr bool operator!=(const expected<T1, E1>& x, const expected<T2, E2>& y);

    // �.�.4.8, Comparison with T
    // template<class T1, class E1, class T2>
    //     friend constexpr bool operator==(const expected<T1, E1>&, const T2&);
    // template<class T1, class E1, class T2>
    //     friend constexpr bool operator==(const T2&, const expected<T1, E1>&);
    // template<class T1, class E1, class T2>
    //     friend constexpr bool operator!=(const expected<T1, E1>&, const T2&);
    // template<class T1, class E1, class T2>
    //     friend constexpr bool operator!=(const T2&, const expected<T1, E1>&);

    // �.�.4.9, Comparison with unexpected<E>
    // template<class T1, class E1, class E2>
    //     friend constexpr bool operator==(const expected<T1, E1>&, const unexpected<E2>&);
    // template<class T1, class E1, class E2>
    //     friend constexpr bool operator==(const unexpected<E2>&, const expected<T1, E1>&);
    // template<class T1, class E1, class E2>
    //     friend constexpr bool operator!=(const expected<T1, E1>&, const unexpected<E2>&);
    // template<class T1, class E1, class E2>
    //     friend constexpr bool operator!=(const unexpected<E2>&, const expected<T1, E1>&);

    // �.�.4.10, Specialized algorithms
    // template<class T1, class E1>
    //    friend void swap(expected<T1, E1>&, expected<T1, E1>&) noexcept(see below);

private:
    storage_type m_storage;
};
}  // namespace wge

#endif  // WOTH_WGE_ERROR_EXPECTED_HPP
