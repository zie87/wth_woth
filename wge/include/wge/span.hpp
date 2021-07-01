#ifndef WOTH_WTH_SPAN_HPP
#define WOTH_WTH_SPAN_HPP

#include <wge/types.hpp>
#include <wge/utils/type_traits.hpp>

#include <iterator>
#include <type_traits>
#include <limits>
#include <array>

namespace wge {
inline constexpr wge::size_t dynamic_extent = std::numeric_limits<wge::size_t>::max();

template <typename T, wge::size_t Extent = wge::dynamic_extent>
class span final {
public:
    using element_type = T;
    using value_type   = std::remove_cv_t<T>;
    using size_type    = wge::size_t;

    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using iterator         = pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;

    static constexpr size_type extent = Extent;

    constexpr span() noexcept            = default;
    constexpr span(const span&) noexcept = default;

    constexpr span(pointer ptr, size_type count) : m_data(ptr), m_size(count) {}
    constexpr span(pointer f, pointer l) : m_data(f), m_size(static_cast<size_type>(f, l)) {}

    template <size_t N>
    constexpr span(element_type (&arr)[N]) noexcept : m_data(arr), m_size(N) {}

    template <size_t N>
    constexpr span(std::array<value_type, N>& arr) noexcept : m_data(arr.data()), m_size(N) {}

    template <size_t N>
    constexpr span(const std::array<value_type, N>& arr) noexcept : m_data(arr.data()), m_size(N) {}

    constexpr span& operator=(const span&) noexcept = default;

    constexpr size_type size() const noexcept { return m_size; }
    constexpr size_type size_byte() const noexcept { return m_size * sizeof(element_type); }

    [[nodiscard]] constexpr bool empty() const noexcept { return (size() == 0U); }

    constexpr iterator begin() const noexcept { return data(); }
    constexpr iterator end() const noexcept { return data() + size(); }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(data() + size()); }
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(data()); }

    constexpr pointer data() const noexcept { return m_data; }

    constexpr reference front() const { return *begin(); }
    constexpr reference back() const { return *end(); }
    constexpr reference operator[](size_type idx) const { return data()[idx]; }

private:
    pointer m_data   = nullptr;
    size_type m_size = 0U;
};

}  // namespace wge

#endif  // WOTH_WTH_SPAN_HPP
