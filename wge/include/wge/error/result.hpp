#ifndef WOTH_WGE_ERROR_RESULT_HPP
#define WOTH_WGE_ERROR_RESULT_HPP

#include <type_traits>
#include <utility>

namespace wge {

namespace detail {

template <typename T, typename E>
class storage_impl {
public:
    using value_type = T;
    using error_type = E;

    constexpr storage_impl() noexcept {}
    ~storage_impl() noexcept {}

    explicit constexpr storage_impl(bool has_value) noexcept : m_has_value(has_value) {}

    void construct_value(value_type const& e) { new (std::addressof(m_value)) value_type(e); }
    void construct_value(value_type&& e) { new (std::addressof(m_value)) value_type(std::move(e)); }

    void construct_error(error_type const& e) { new (std::addressof(m_error)) error_type(e); }
    void construct_error(error_type&& e) { new (std::addressof(m_error)) error_type(std::move(e)); }

    void destruct_value() { m_value.~value_type(); }
    void destruct_error() { m_error.~error_type(); }

    template <class... Args>
    void emplace_value(Args&&... args) {
        new (std::addressof(m_value)) value_type(std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    void emplace_value(std::initializer_list<U> il, Args&&... args) {
        new (std::addressof(m_value)) value_type(il, std::forward<Args>(args)...);
    }

    template <class... Args>
    void emplace_error(Args&&... args) {
        new (std::addressof(m_error)) error_type(std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    void emplace_error(std::initializer_list<U> il, Args&&... args) {
        new (std::addressof(m_error)) error_type(il, std::forward<Args>(args)...);
    }

    constexpr auto value() const& noexcept -> const value_type& { return m_value; }
    constexpr auto value() & noexcept -> value_type& { return m_value; }
    constexpr auto value() const&& noexcept -> const value_type&& { return std::move(m_value); }
    constexpr auto value() && noexcept -> value_type&& { return std::move(m_value); }

    constexpr auto error() const& noexcept -> const error_type& { return m_error; }
    constexpr auto error() & noexcept -> error_type& { return m_error; }
    constexpr auto error() const&& noexcept -> const error_type&& { return std::move(m_error); }
    constexpr auto error() && noexcept -> error_type&& { return std::move(m_error); }

    auto value_ptr() const noexcept -> const value_type* { return std::addressof(m_value); }
    auto value_ptr() noexcept -> value_type* { return std::addressof(m_value); }

    constexpr bool has_value() const noexcept { return m_has_value; }
    void set_has_value(bool v) { m_has_value = v; }

private:
    union {
        value_type m_value;
        error_type m_error;
    };
    bool m_has_value = false;
};

template <typename E>
class storage_impl<void, E> {
public:
    using error_type = E;

    void construct_error(const error_type& e) { new (std::addressof(m_error)) error_type(e); }
    void construct_error(error_type&& e) { new (std::addressof(m_error)) error_type(std::move(e)); }

    void destruct_error() { m_error.~error_type(); }

    template <class... Args>
    void emplace_error(Args&&... args) {
        new (std::addressof(m_error)) error_type(std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    void emplace_error(std::initializer_list<U> il, Args&&... args) {
        new (std::addressof(m_error)) error_type(il, std::forward<Args>(args)...);
    }

    constexpr auto error() const& noexcept -> const error_type& { return m_error; }
    constexpr auto error() & noexcept -> error_type& { return m_error; }
    constexpr auto error() const&& noexcept -> const error_type&& { return std::move(m_error); }
    constexpr auto error() && noexcept -> error_type&& { return std::move(m_error); }

    constexpr bool has_value() const noexcept { return m_has_value; }
    void set_has_value(bool v) { m_has_value = v; }

protected:
    constexpr storage_impl() noexcept {}
    ~storage_impl() noexcept {}

    explicit constexpr storage_impl(bool has_value) noexcept : m_has_value(has_value) {}

private:
    union {
        char m_dummy;
        error_type m_error;
    };
    bool m_has_value = false;
};

template <typename T, typename E, bool IsConstructable, bool IsMoveable>
class storage_t;

template <typename T, typename E>
class storage_t<T, E, true, true> final : public storage_impl<T, E> {
public:
    constexpr storage_t() noexcept = default;
    ~storage_t() noexcept          = default;

    explicit constexpr storage_t(bool has_value) noexcept : storage_impl<T, E>(has_value) {}

    storage_t(const storage_t& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(other.value());
        } else {
            this->construct_error(other.error());
        }
    }

    storage_t(storage_t&& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(std::move(other.value()));
        } else {
            this->construct_error(std::move(other.error()));
        }
    }
};

template <typename E>
class storage_t<void, E, true, true> final : public storage_impl<void, E> {
public:
    constexpr storage_t() noexcept = default;
    ~storage_t() noexcept          = default;

    explicit constexpr storage_t(bool has_value) noexcept : storage_impl<void, E>(has_value) {}

    storage_t(const storage_t& other) : storage_impl<void, E>(other.has_value()) {
        if (!this->has_value()) {
            this->construct_error(other.error());
        }
    }

    storage_t(storage_t&& other) : storage_impl<void, E>(other.has_value()) {
        if (!this->has_value()) {
            this->construct_error(std::move(other.error()));
        }
    }
};

template <typename T, typename E>
class storage_t<T, E, true, false> final : public storage_impl<T, E> {
public:
    constexpr storage_t() noexcept = default;
    ~storage_t() noexcept          = default;

    explicit constexpr storage_t(bool has_value) noexcept : storage_impl<T, E>(has_value) {}

    storage_t(const storage_t& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(other.value());
        } else {
            this->construct_error(other.error());
        }
    }
    storage_t(storage_t&& other) = delete;
};

template <typename E>
class storage_t<void, E, true, false> final : public storage_impl<void, E> {
public:
    constexpr storage_t() noexcept = default;
    ~storage_t() noexcept          = default;

    explicit constexpr storage_t(bool has_value) noexcept : storage_impl<void, E>(has_value) {}

    storage_t(const storage_t& other) : storage_impl<void, E>(other.has_value()) {
        if (!this->has_value()) {
            this->construct_error(other.error());
        }
    }
    storage_t(storage_t&& other) = delete;
};

template <typename T, typename E>
class storage_t<T, E, false, true> final : public storage_impl<T, E> {
public:
    constexpr storage_t() noexcept = default;
    ~storage_t() noexcept          = default;

    explicit constexpr storage_t(bool has_value) noexcept : storage_impl<T, E>(has_value) {}

    storage_t(const storage_t& other) = delete;
    storage_t(storage_t&& other) : storage_impl<T, E>(other.has_value()) {
        if (this->has_value()) {
            this->construct_value(std::move(other.value()));
        } else {
            this->construct_error(std::move(other.error()));
        }
    }
};

template <typename E>
class storage_t<void, E, false, true> final : public storage_impl<void, E> {
public:
    constexpr storage_t() noexcept = default;
    ~storage_t() noexcept          = default;

    explicit constexpr storage_t(bool has_value) noexcept : storage_impl<void, E>(has_value) {}

    storage_t(const storage_t& other) = delete;
    storage_t(storage_t&& other) : storage_impl<void, E>(other.has_value()) {
        if (!this->has_value()) {
            this->construct_error(std::move(other.error()));
        }
    }
};

}  // namespace detail

template <typename T, typename E>
class result_base {
public:
    using value_type = T;
    using error_type = E;

    template <bool B = std::is_default_constructible<T>::value, typename std::enable_if<B, int>::type = 0>
    constexpr result_base() : m_storage(true) {
        m_storage.construct_value(value_type());
    }
    ~result_base() {
        if (has_value()) {
            m_storage.destruct_value();
        } else {
            m_storage.destruct_error();
        }
    }

    constexpr result_base(const result_base&)     = default;
    constexpr result_base(result_base&&) noexcept = default;

    template <typename U = T, typename std::enable_if<std::is_copy_constructible<U>::value, int>::type = 0>
    constexpr result_base(const value_type& value) : m_storage(true) {
        m_storage.construct_value(value);
    }

    constexpr explicit operator bool() const noexcept { return has_value(); }
    constexpr bool has_value() const noexcept { return m_storage.has_value(); }

private:
    using storage_type =
        detail::storage_t<T, E, std::is_copy_constructible<T>::value && std::is_copy_constructible<E>::value,
                          std::is_move_constructible<T>::value && std::is_move_constructible<E>::value>;
    storage_type m_storage;
};

}  // namespace wge

#endif  // WOTH_WGE_ERROR_RESULT_HPP

//#define nsel_REQUIRES_0(...) template <bool B = (__VA_ARGS__), typename std::enable_if<B, int>::type = 0>
//#define nsel_REQUIRES_T(...) , typename std::enable_if<(__VA_ARGS__), int>::type = 0
//#define nsel_REQUIRES_R(R, ...) typename std::enable_if<(__VA_ARGS__), R>::type
//#define nsel_REQUIRES_A(...) , typename std::enable_if<(__VA_ARGS__), void*>::type = nullptr
