#ifndef WGE_VIDEO_IMAGE_LOADER_HPP
#define WGE_VIDEO_IMAGE_LOADER_HPP

#include <istream>
#include <string>

#include <wge/memory.hpp>
#include <wge/types.hpp>

#include <wge/video/pixel_format.hpp>
#include <wge/video/vram_ptr.hpp>

namespace wge {
namespace video {
struct texture_data {
    texture_data() noexcept = default;
    explicit texture_data(wge::size_t w, wge::size_t h, wge::size_t tw, wge::size_t th,
                          wge::owner_ptr<wge::byte_t*> buf, wge::size_t channels) noexcept;

    explicit texture_data(wge::size_t w, wge::size_t h, wge::size_t tw, wge::size_t th, vram_ptr<wge::byte_t>&& buf,
                          wge::size_t channels) noexcept;

    wge::size_t width = 0;
    wge::size_t height = 0;

    wge::size_t texture_width = 0;
    wge::size_t texture_height = 0;

    vram_ptr<wge::byte_t> pixels = {};
    wge::size_t channels = 0;
};

struct image_loader {
    static const int number_of_channels;

    //static texture_data load_image(std::istream& stream) noexcept;
    static texture_data load_png(std::istream& stream) noexcept;
    //static texture_data load_jpeg(std::istream& stream) noexcept;

    static texture_data load_jpeg(const wge::byte_t* const buffer, wge::size_t buffer_size, pixel_format format,
                                  bool use_vram, bool swizzle) noexcept;
    

    static texture_data load_image(const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept;
    static texture_data load_png(const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept;

    static inline texture_data load_jpeg(const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept {
        return load_jpeg(buffer, buffer_size, pixel_format::none, false, false);
    }
};
}  // namespace video
}  // namespace wge

#endif  // WGE_GRAPHIC_IMAGE_LOADER_HPP
