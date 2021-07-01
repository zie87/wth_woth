#ifndef WOTH_WTH_SPAN_HPP
#define WOTH_WTH_SPAN_HPP

#include <wge/types.hpp>
#include <wge/debug/assert.hpp>
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
    constexpr span(pointer f, pointer l) : span(f, static_cast<size_type>(std::distance(f, l))) {}

    template <size_t N>
    explicit constexpr span(element_type (&arr)[N]) noexcept : span(arr, N) {}

    template <size_t N>
    explicit constexpr span(std::array<value_type, N>& arr) noexcept : span(arr.data(), N) {}

    template <size_t N>
    explicit constexpr span(const std::array<value_type, N>& arr) noexcept : span(arr.data(), N) {}

    constexpr span& operator=(const span&) noexcept = default;

    constexpr size_type size() const noexcept { return m_size; }
    constexpr size_type size_byte() const noexcept { return m_size * sizeof(element_type); }

    [[nodiscard]] constexpr bool empty() const noexcept { return (size() == 0U); }

    constexpr iterator begin() const noexcept { return data(); }
    constexpr iterator end() const noexcept { return data() + size(); }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    constexpr pointer data() const noexcept { return m_data; }

    constexpr reference front() const { return *begin(); }
    constexpr reference back() const { return *(end()- 1); }
    constexpr reference operator[](size_type idx) const { return data()[idx]; }

    template <wge::size_t Count>
    constexpr auto first() const noexcept -> span<element_type, Count> {
        wge_expects(Count <= size());
        return {data(), Count};
    }
    constexpr auto first(size_type count) const noexcept -> span<element_type, wge::dynamic_extent> {
        wge_expects(count <= size());
        return {data(), count};
    }

    template <wge::size_t Count>
    constexpr auto last() const noexcept -> span<element_type, Count> {
        wge_expects(Count <= size());
        return {data() + size() - Count, Count};
    }
    constexpr auto last(size_type count) const noexcept -> span<element_type, wge::dynamic_extent> {
        wge_expects(count <= size());
        return {data() + size() - count, count};
    }

    template <size_t Offset, size_t Count = wge::dynamic_extent>
    constexpr span<element_type, Count> subspan() const noexcept {
        wge_expects(Offset <= size());
        wge_expects(Count == wge::dynamic_extent || Count <= size());
        constexpr size_type cnt = (Count == wge::dynamic_extent ? size() - Offset : Count);
        return span<element_type, Count>{data() + Offset, cnt};
    }

    constexpr span<element_type, wge::dynamic_extent> subspan(size_type offset,
                                                              size_type count = wge::dynamic_extent) const noexcept {
        wge_expects(offset <= size());
        wge_expects(count <= size() || count == wge::dynamic_extent);

        if (count == dynamic_extent) {
            return {data() + offset, size() - offset};
        }

        wge_expects(count <= size() - offset);
        return {data() + offset, count};
    }

private:
    pointer m_data   = nullptr;
    size_type m_size = 0U;
};

template <class T, wge::size_t N>
auto as_bytes(wge::span<T, N> s) noexcept
    -> wge::span<const wge::byte_t, (N == wge::dynamic_extent) ? wge::dynamic_extent : N * sizeof(T)> {
    return {reinterpret_cast<const wge::byte_t*>(s.data()), s.size_byte()};
}

template <class T, wge::size_t N>
auto as_writable_bytes(wge::span<T, N> s) noexcept
    -> std::enable_if_t<!std::is_const_v<T>,
                        wge::span<wge::byte_t, (N == wge::dynamic_extent) ? wge::dynamic_extent : N * sizeof(T)>> {
    return {reinterpret_cast<wge::byte_t*>(s.data()), s.size_byte()};
}

}  // namespace wge

#endif  // WOTH_WTH_SPAN_HPP
