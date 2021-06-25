#ifndef WGE_VIDEO_PIXELFORMAT_HPP
#define WGE_VIDEO_PIXELFORMAT_HPP

#include <wge/types.hpp>

namespace wge {
namespace video {

enum class pixel_format {
    none,
    format_5650,
    format_5551,
    format_4444,
    format_8888,
};

template <pixel_format Format>
struct pixel_converter;

template <>
struct pixel_converter<pixel_format::none> final {
    using pixel_t = wge::u32;
    static constexpr auto pixel_size = sizeof(pixel_t);

    static constexpr pixel_t convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_t>(r | (g << 8) | (b << 16) | (a << 24));
    }
};

template <>
struct pixel_converter<pixel_format::format_5650> final {
    using pixel_t = wge::u16;
    static constexpr auto pixel_size = sizeof(pixel_t);

    static constexpr pixel_t convert(wge::u32 r, wge::u32 g, wge::u32 b, [[maybe_unused]] wge::u32 a = 255) noexcept {
        return static_cast<pixel_t>((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11));
    }
};

template <>
struct pixel_converter<pixel_format::format_5551> final {
    using pixel_t = wge::u16;
    static constexpr auto pixel_size = sizeof(pixel_t);

    static constexpr pixel_t convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_t>((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10) | ((a >> 7) << 15));
    }
};

template <>
struct pixel_converter<pixel_format::format_4444> final {
    using pixel_t = wge::u16;
    static constexpr auto pixel_size = sizeof(pixel_t);

    static constexpr pixel_t convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_t>((r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8) | ((a >> 4) << 12));
    }
};

template <>
struct pixel_converter<pixel_format::format_8888> final {
    using pixel_t = wge::u32;
    static constexpr auto pixel_size = sizeof(pixel_t);

    static constexpr pixel_t convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_t>(r | (g << 8) | (b << 16) | (a << 24));
    }
};

inline constexpr wge::size_t get_pixel_size(pixel_format format) noexcept {
    switch (format) {
    case pixel_format::format_5650:
        return pixel_converter<pixel_format::format_5650>::pixel_size;
    case pixel_format::format_5551:
        return pixel_converter<pixel_format::format_5551>::pixel_size;
    case pixel_format::format_4444:
        return pixel_converter<pixel_format::format_4444>::pixel_size;
    case pixel_format::format_8888:
        return pixel_converter<pixel_format::format_8888>::pixel_size;
    case pixel_format::none:
        return pixel_converter<pixel_format::none>::pixel_size;
    }

    return get_pixel_size(pixel_format::none);
}

}  // namespace video
}  // namespace wge

#endif  // WGE_VIDEO_PIXELFORMAT_HPP
