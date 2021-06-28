#ifndef WOTH_WGE_ERROR_RESULT_HPP
#define WOTH_WGE_ERROR_RESULT_HPP

#include <wge/string/view.hpp>

#include <type_traits>
#include <utility>
#include <exception>

#include <cstdio>

namespace wge {

namespace detail {

template <typename V>
struct result_value {
    result_value(const V& v) : val(v) {}
    result_value(V&& v) noexcept : val(std::move(v)) {}

    V val;
};

template <typename E>
struct result_error {
    result_error(const E& e) : err(e) {}
    result_error(E&& e) noexcept : err(std::move(e)) {}

    E err;
};

template <typename V, typename E>
struct storage {
    static constexpr auto size  = sizeof(V) > sizeof(E) ? sizeof(V) : sizeof(E);
    static constexpr auto align = sizeof(V) > sizeof(E) ? alignof(V) : alignof(E);

    using type = typename std::aligned_storage_t<size, align>;

    void construct(const detail::result_value<V>& value) {
        new (&m_storage) V(value.val);
        m_is_constructed = true;
    }

    void construct(detail::result_value<V>&& value) {
        new (&m_storage) V(std::move(value.val));
        m_is_constructed = true;
    }

    void construct(const detail::result_error<E>& error) {
        new (&m_storage) E(error.err);
        m_is_constructed = true;
    }

    void construct(detail::result_error<E>&& error) {
        new (&m_storage) E(std::move(error.err));
        m_is_constructed = true;
    }

    template <typename U>
    auto get() const& noexcept -> const U& {
        return *reinterpret_cast<const U*>(&m_storage);
    }

    template <typename U>
    auto get() & noexcept -> U& {
        return *reinterpret_cast<U*>(&m_storage);
    }

private:
    bool m_is_constructed = false;
    type m_storage        = {};
};

struct terminate_on_error {
    static void handle_error(const char* msg) noexcept {
        std::fprintf(stderr, msg);
        std::terminate();
    }
};

}  // namespace detail

template <typename Value, typename Error, class ErrorPolicy>
class basic_result_t {
    static_assert(!std::is_same<Error, void>::value, "void error type is not allowed");

    using storage_type = typename detail::storage<Value, Error>;

    using result_value_type = detail::result_value<Value>;
    using result_error_type = detail::result_error<Error>;

public:
    using value_type = Value;
    using error_type = Error;

    basic_result_t(const value_type& val) : basic_result_t(result_value_type(val)) {}
    basic_result_t(value_type&& val) : basic_result_t(result_value_type(std::move(val))) {}

    basic_result_t(result_value_type val) : m_is_value(true) { m_storage.construct(std::move(val)); }
    basic_result_t(result_error_type err) : m_is_value(false) { m_storage.construct(std::move(err)); }

    [[nodiscard]] constexpr bool has_value() const noexcept { return m_is_value; }
    [[nodiscard]] constexpr bool has_error() const noexcept { return !(has_value()); }

    constexpr explicit operator bool() const noexcept { return has_value(); }

    template <typename U = Value, std::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr auto value() const& -> const U& {
        if (has_error()) {
            ErrorPolicy::handle_error("Attempting to unwrap an value on error Result\n");
        }

        return get_value();
    }

    template <typename U = Value, std::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr auto value() & -> U& {
        if (has_error()) {
            ErrorPolicy::handle_error("Attempting to unwrap an value on error Result\n");
        }

        return get_value();
    }

    template <typename U = Value, std::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr auto value() && -> U&& {
        if (has_error()) {
            ErrorPolicy::handle_error("Attempting to unwrap an value on error Result\n");
        }

        return std::move(get_value());
    }

    constexpr auto error() const& -> const error_type& {
        if (has_value()) {
            ErrorPolicy::handle_error("Attempting to unwrap an error on value result\n");
        }

        return get_error();
    }

    constexpr auto error() & -> error_type& {
        if (has_value()) {
            ErrorPolicy::handle_error("Attempting to unwrap an error on value result\n");
        }

        return get_error();
    }

    constexpr auto error() && -> error_type&& {
        if (has_value()) {
            ErrorPolicy::handle_error("Attempting to unwrap an error on value result\n");
        }

        return std::move(get_error());
    }

private:
    template <typename U = Value, std::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr auto get_value() const noexcept -> const U& {
        return m_storage.template get<U>();
    }

    template <typename U = Value, std::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr auto get_value() noexcept -> U& {
        return m_storage.template get<U>();
    }

    constexpr auto get_error() const noexcept -> const error_type& { return m_storage.template get<error_type>(); }

    constexpr auto get_error() noexcept -> error_type& { return m_storage.template get<error_type>(); }

    bool m_is_value = false;
    storage_type m_storage{};
};

template <class E>
detail::result_error<typename std::decay<E>::type> make_error_result(E&& e) {
    return detail::result_error<typename std::decay<E>::type>(std::forward<E>(e));
}

template <typename Value, typename Error>
using result_t = basic_result_t<Value, Error, detail::terminate_on_error>;

}  // namespace wge

#endif  // WOTH_WGE_ERROR_RESULT_HPP
