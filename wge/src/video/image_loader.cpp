#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

#include <wge/video/image_loader.hpp>
#include <wge/math/utils.hpp>
#include <wge/types.hpp>

#include <vector>

namespace wge {
namespace video {
const int image_loader::number_of_channels = 4;

texture_data::texture_data(wge::size_t w, wge::size_t h, wge::size_t tw, wge::size_t th, wge::byte_t* buf,
                           wge::size_t channels) noexcept
    : width(w), height(h), texture_width(tw), texture_height(th), pixels(buf), channels(channels) {}

namespace {
static int stbi_cb_read(void* user, char* data, int size) {
    auto* fs = reinterpret_cast<std::istream*>(user);
    fs->read(data, size);
    return fs->gcount();
}

static void stbi_cb_skip(void* user, int n) {
    auto* fs = reinterpret_cast<std::istream*>(user);
    fs->seekg(n, std::ios_base::cur);
}

static int stbi_cb_eof(void* user) {
    auto* fs = reinterpret_cast<std::istream*>(user);
    return fs->eof() ? 1 : 0;
}
}  // namespace

texture_data image_loader::load_image(std::istream& stream) {
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_io_callbacks cb = {stbi_cb_read, stbi_cb_skip, stbi_cb_eof};

    auto* pixels = stbi_load_from_callbacks(&cb, &stream, &width, &height, &channels, 0);

    int new_width = 0;
    int new_height = 0;
    auto* texture_buffer = convert_pixel_buffer(pixels, width, height, channels, new_width, new_height);

    stbi_image_free(pixels);

    return texture_data(width, height, new_width, new_height, texture_buffer, number_of_channels);
}

texture_data image_loader::load_image(const wge::byte_t* const buffer, wge::size_t buffer_size) {
    int width = 0;
    int height = 0;
    int channels = 0;

    auto buf_len = static_cast<int>(buffer_size);
    auto* pixels = stbi_load_from_memory(buffer, buf_len, &width, &height, &channels, 0);

    int new_width = 0;
    int new_height = 0;
    auto* texture_buffer = convert_pixel_buffer(pixels, width, height, channels, new_width, new_height);

    stbi_image_free(pixels);

    return texture_data(width, height, new_width, new_height, texture_buffer, number_of_channels);
}

wge::byte_t* image_loader::convert_pixel_buffer(const wge::byte_t* const buffer, const int width, const int height,
                                                const int channels, int& new_width, int& new_height) {
    new_width = math::nearest_superior_power_of_2(width);
    new_height = math::nearest_superior_power_of_2(height);

    auto texture_data = new unsigned char[new_height * new_width * 4]();

    for (int line = 0; line < height; ++line) {
        auto line_ptr = buffer + (width * channels * line);
        auto curr_row = texture_data + (new_width * number_of_channels * line);

        for (int row = 0; row < width; ++row) {
            auto target_pos = row * number_of_channels;
            auto source_pos = row * channels;

            for (int color_pos = 0; color_pos < number_of_channels; ++color_pos) {
                if (channels < (color_pos + 1)) {
                    curr_row[target_pos + color_pos] = 255;
                } else {
                    curr_row[target_pos + color_pos] = line_ptr[source_pos + color_pos];
                }
            }
        }
    }
    return texture_data;
}
}  // namespace video
}  // namespace wge
