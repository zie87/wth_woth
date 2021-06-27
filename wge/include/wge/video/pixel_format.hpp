#ifndef WGE_VIDEO_PIXELFORMAT_HPP
#define WGE_VIDEO_PIXELFORMAT_HPP

#include <wge/types.hpp>

namespace wge {
namespace video {

enum class pixel_format {
    none,
    bgr_5650,
    abgr_5551,
    abgr_4444,
    abgr_8888,
};

template <pixel_format Format>
struct pixel_converter;

template <>
struct pixel_converter<pixel_format::none> final {
    using pixel_type                 = wge::u32;
    static constexpr auto pixel_size = sizeof(pixel_type);

    static constexpr pixel_type convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_type>(r | (g << 8) | (b << 16) | (a << 24));
    }
};

template <>
struct pixel_converter<pixel_format::bgr_5650> final {
    using pixel_type                 = wge::u16;
    static constexpr auto pixel_size = sizeof(pixel_type);

    static constexpr pixel_type convert(wge::u32 r, wge::u32 g, wge::u32 b,
                                       [[maybe_unused]] wge::u32 a = 255) noexcept {
        return static_cast<pixel_type>((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11));
    }
};

template <>
struct pixel_converter<pixel_format::abgr_5551> final {
    using pixel_type                 = wge::u16;
    static constexpr auto pixel_size = sizeof(pixel_type);

    static constexpr pixel_type mask_alpha = 0x8000;
    static constexpr pixel_type mask_blue  = 0x7C00;
    static constexpr pixel_type mask_green = 0x03E0;
    static constexpr pixel_type mask_red   = 0x001F;

    static constexpr pixel_type convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_type>((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10) | ((a >> 7) << 15));
    }
};

template <>
struct pixel_converter<pixel_format::abgr_4444> final {
    using pixel_type                 = wge::u16;
    static constexpr auto pixel_size = sizeof(pixel_type);

    static constexpr pixel_type mask_alpha = 0xF000;
    static constexpr pixel_type mask_blue  = 0x0F00;
    static constexpr pixel_type mask_green = 0x00F0;
    static constexpr pixel_type mask_red   = 0x000F;

    static constexpr pixel_type convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_type>((r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8) | ((a >> 4) << 12));
    }
};

template <>
struct pixel_converter<pixel_format::abgr_8888> final {
    using pixel_type                 = wge::u32;
    static constexpr auto pixel_size = sizeof(pixel_type);

    static constexpr pixel_type mask_alpha = 0xFF000000;
    static constexpr pixel_type mask_blue  = 0x00FF0000;
    static constexpr pixel_type mask_green = 0x0000FF00;
    static constexpr pixel_type mask_red   = 0x000000FF;

    static constexpr pixel_type convert(wge::u32 r, wge::u32 g, wge::u32 b, wge::u32 a = 255) noexcept {
        return static_cast<pixel_type>(r | (g << 8) | (b << 16) | (a << 24));
    }
};

inline constexpr wge::size_t get_pixel_size(pixel_format format) noexcept {
    switch (format) {
    case pixel_format::bgr_5650:
        return pixel_converter<pixel_format::bgr_5650>::pixel_size;
    case pixel_format::abgr_5551:
        return pixel_converter<pixel_format::abgr_5551>::pixel_size;
    case pixel_format::abgr_4444:
        return pixel_converter<pixel_format::abgr_4444>::pixel_size;
    case pixel_format::abgr_8888:
        return pixel_converter<pixel_format::abgr_8888>::pixel_size;
    case pixel_format::none:
        return pixel_converter<pixel_format::none>::pixel_size;
    }

    return get_pixel_size(pixel_format::none);
}

}  // namespace video
}  // namespace wge

#endif  // WGE_VIDEO_PIXELFORMAT_HPP
