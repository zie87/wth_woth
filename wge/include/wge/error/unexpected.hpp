#ifndef WOTH_WGE_ERROR_UNEXPECTED_HPP
#define WOTH_WGE_ERROR_UNEXPECTED_HPP

#include <wge/utils/type_traits.hpp>

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace wge {

template <class E>
class unexpected {
public:
    using error_type = E;

    constexpr unexpected(const unexpected&) = default;
    constexpr unexpected(unexpected&&)      = default;

    template <class Err                          = error_type,
              typename std::enable_if<std::is_constructible_v<error_type, Err> &&
                                          !std::is_same_v<wge::remove_cvref_t<Err>, std::in_place_t> &&
                                          !std::is_same_v<wge::remove_cvref_t<Err>, unexpected>,
                                      int>::type = 0>
    constexpr explicit unexpected(Err&& e) : m_value(std::forward<Err>(e)) {}

    template <class... Args, typename std::enable_if<std::is_constructible_v<error_type, Args&&...>, int>::type = 0>
    constexpr explicit unexpected(std::in_place_t, Args&&... args) : m_value(std::forward<Args>(args)...) {}

    template <class U, class... Args,
              typename std::enable_if<std::is_constructible_v<error_type, std::initializer_list<U>, Args&&...>,
                                      int>::type = 0>
    constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> init_list, Args&&... args)
        : m_value(init_list, std::forward<Args>(args)...) {}

    template <class Err,
              typename std::enable_if<std::is_constructible_v<error_type, const Err&> &&
                                          !std::is_constructible_v<error_type, unexpected<Err>&> &&
                                          !std::is_constructible_v<error_type, unexpected<Err> > &&
                                          !std::is_constructible_v<error_type, unexpected<Err> const&> &&
                                          !std::is_constructible_v<error_type, unexpected<Err> const> &&
                                          !std::is_convertible_v<unexpected<Err>&, error_type> &&
                                          !std::is_convertible_v<unexpected<Err>, error_type> &&
                                          !std::is_convertible_v<const unexpected<Err>&, error_type> &&
                                          !std::is_convertible_v<const unexpected<Err>, error_type> &&
                                          !std::is_convertible_v<const Err&, error_type>, /* explicit condition*/
                                      int>::type = 0>
    constexpr explicit unexpected(const unexpected<Err>& e) : m_value(e.value()) {}

    template <class Err, typename std::enable_if<std::is_constructible_v<error_type, const Err&> &&
                                                     !std::is_constructible_v<error_type, unexpected<Err>&> &&
                                                     !std::is_constructible_v<error_type, unexpected<Err> > &&
                                                     !std::is_constructible_v<error_type, unexpected<Err> const&> &&
                                                     !std::is_constructible_v<error_type, unexpected<Err> const> &&
                                                     !std::is_convertible_v<unexpected<Err>&, error_type> &&
                                                     !std::is_convertible_v<unexpected<Err>, error_type> &&
                                                     !std::is_convertible_v<const unexpected<Err>&, error_type> &&
                                                     !std::is_convertible_v<const unexpected<Err>, error_type> &&
                                                     std::is_convertible_v<Err, error_type>, /* explicit condition*/
                                                 int>::type = 0>
    constexpr /* non explicit*/ unexpected(const unexpected<Err>& e) : m_value(e.value()) {}

    template <class Err, typename std::enable_if<std::is_constructible_v<error_type, Err> &&
                                                     !std::is_constructible_v<error_type, unexpected<Err>&> &&
                                                     !std::is_constructible_v<error_type, unexpected<Err> > &&
                                                     !std::is_constructible_v<error_type, unexpected<Err> const&> &&
                                                     !std::is_constructible_v<error_type, unexpected<Err> const> &&
                                                     !std::is_convertible_v<unexpected<Err>&, error_type> &&
                                                     !std::is_convertible_v<unexpected<Err>, error_type> &&
                                                     !std::is_convertible_v<const unexpected<Err>&, error_type> &&
                                                     !std::is_convertible_v<const unexpected<Err>, error_type> &&
                                                     !std::is_convertible_v<Err, error_type>, /* explicit condition*/
                                                 int>::type = 0>
    constexpr explicit unexpected(unexpected<Err>&& e) : m_value(std::move(e.value())) {}

    template <class Err,
              typename std::enable_if<std::is_constructible_v<error_type, Err> &&
                                          !std::is_constructible_v<error_type, unexpected<Err>&> &&
                                          !std::is_constructible_v<error_type, unexpected<Err> > &&
                                          !std::is_constructible_v<error_type, unexpected<Err> const&> &&
                                          !std::is_constructible_v<error_type, unexpected<Err> const> &&
                                          !std::is_convertible_v<unexpected<Err>&, error_type> &&
                                          !std::is_convertible_v<unexpected<Err>, error_type> &&
                                          !std::is_convertible_v<const unexpected<Err>&, error_type> &&
                                          !std::is_convertible_v<const unexpected<Err>, error_type> &&
                                          std::is_convertible_v<const Err&, error_type>, /* explicit condition*/
                                      int>::type = 0>
    constexpr /* non explicit*/ unexpected(unexpected<Err>&& e) : m_value(std::move(e.value())) {}

    constexpr unexpected& operator=(const unexpected&) = default;
    constexpr unexpected& operator=(unexpected&&) = default;

    template <
        class Err =
            error_type /*, typename std::enable_if_t<std::is_assignable_v<error_type, const Err&>, int>::type = 0*/>
    constexpr unexpected& operator=(const unexpected<Err>& other) {
        unexpected{other.value()}.swap(*this);
        return *this;
    }

    template <class Err                                                                        = error_type,
              typename std::enable_if_t<std::is_move_assignable_v<error_type, Err>, int>::type = 0>
    constexpr unexpected& operator=(unexpected<Err>&& other) {
        unexpected{std::move(other.value())}.swap(*this);
        return *this;
    }

    constexpr auto value() const& noexcept -> const error_type& { return m_value; }
    constexpr auto value() & noexcept -> error_type& { return m_value; }
    constexpr auto value() const&& noexcept -> const error_type&& { return std::move(m_value); }
    constexpr auto value() && noexcept -> error_type&& { return std::move(m_value); }

    auto swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<error_type>)
        -> std::enable_if_t<std::is_swappable_v<error_type>, void> {
        using std::swap;
        swap(m_value, other.m_value);
    }

private:
    E m_value;
};
template <class E1, class E2>
constexpr bool operator==(const unexpected<E1>& x, const unexpected<E2>& y) {
    return x.value() == y.value();
}

template <class E1, class E2>
constexpr bool operator!=(const unexpected<E1>& x, const unexpected<E2>& y) {
    return !(x == y);
}

template <class E1>
auto swap(unexpected<E1>& x, unexpected<E1>& y) noexcept(noexcept(x.swap(y)))
    -> std::enable_if_t<std::is_swappable_v<E1>, void> {
    x.swap(y);
}
}  // namespace wge

#endif  // WOTH_WGE_ERROR_UNEXPECTED_HPP
