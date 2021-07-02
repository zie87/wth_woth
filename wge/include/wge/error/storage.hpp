#ifndef WOTH_WGE_ERROR_STORAGE_HPP
#define WOTH_WGE_ERROR_STORAGE_HPP

#include <memory>
#include <utility>

namespace wge {
namespace detail {

struct no_init_t {};
static constexpr no_init_t no_init{};

template <typename T, typename E>
struct storage_impl {
    using value_type = T;
    using error_type = E;

    constexpr storage_impl() noexcept : m_value(value_type{}), m_has_value(true) {}
    constexpr storage_impl(no_init_t) noexcept : m_no_init(), m_has_value(false) {}
    ~storage_impl() noexcept {}

    explicit storage_impl(bool has_value) noexcept : m_has_value(has_value) {}

    constexpr bool has_value() const noexcept { return m_has_value; }
    void set_has_value(bool v) noexcept { m_has_value = v; }

    void construct_value(const value_type& e) { new (value_ptr()) value_type(e); }
    void construct_value(value_type&& e) { new (value_ptr()) value_type(std::move(e)); }

    void construct_error(error_type const& e) { new (error_ptr()) error_type(e); }
    void construct_error(error_type&& e) { new (error_ptr()) error_type(std::move(e)); }

    void destruct_value() { m_value.~value_type(); }
    void destruct_error() { m_error.~error_type(); }

    constexpr auto value() const& noexcept -> const value_type& { return m_value; }
    constexpr auto value() & noexcept -> value_type& { return m_value; }
    constexpr auto value() const&& noexcept -> const value_type&& { return std::move(m_value); }
    constexpr auto value() && noexcept -> value_type&& { return std::move(m_value); }

    constexpr auto error() const& noexcept -> const error_type& { return m_error; }
    constexpr auto error() & noexcept -> error_type& { return m_error; }
    constexpr auto error() const&& noexcept -> const error_type&& { return std::move(m_error); }
    constexpr auto error() && noexcept -> error_type&& { return std::move(m_error); }

    constexpr auto value_ptr() noexcept -> value_type* { return std::addressof(m_value); }
    constexpr auto value_ptr() const noexcept -> const value_type* { return std::addressof(m_value); }

    constexpr auto error_ptr() noexcept -> error_type* { return std::addressof(m_error); }
    constexpr auto error_ptr() const noexcept -> const error_type* { return std::addressof(m_error); }

private:
    union {
        char m_no_init;
        value_type m_value;
        error_type m_error;
    };
    bool m_has_value = false;
};

template <typename T, typename E, bool isConstructable, bool isMoveable>
struct storage;

template <typename T, typename E>
struct storage<T, E, true, true> final : storage_impl<T, E> {
    storage()  = default;
    ~storage() = default;

    explicit storage(bool has_value) noexcept : storage_impl<T, E>(has_value) {}

    storage(storage const& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(other.value());
        } else {
            this->construct_error(other.error());
        }
    }

    storage(storage&& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(std::move(other.value()));
        } else {
            this->construct_error(std::move(other.error()));
        }
    }
};

template <typename T, typename E>
struct storage<T, E, true, false> final : storage_impl<T, E> {
    storage()  = default;
    ~storage() = default;

    explicit storage(bool has_value) noexcept : storage_impl<T, E>(has_value) {}

    storage(const storage& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(other.value());
        } else {
            this->construct_error(other.error());
        }
    }

    storage(storage&& other) = delete;
};

template <typename T, typename E>
struct storage<T, E, false, true> final : storage_impl<T, E> {
    storage()  = default;
    ~storage() = default;

    explicit storage(bool has_value) noexcept : storage_impl<T, E>(has_value) {}

    storage(const storage& other) = delete;

    storage(storage&& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(std::move(other.value()));
        } else {
            this->construct_error(std::move(other.error()));
        }
    }
};

}  // namespace detail
}  // namespace wge

#endif  // WOTH_WGE_ERROR_STORAGE_HPP
