#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

#include <wge/video/image_loader.hpp>
#include <wge/math/utils.hpp>
#include <wge/types.hpp>

#include <vector>
#include <cstring>

namespace {

enum class image_types {
    jpg,      // joint photographic experts group - .jpeg or .jpg
    png,      // portable network graphics
    gif,      // graphics interchange format
    tiff,     // tagged image file format
    bmp,      // Microsoft bitmap format
    webp,     // Google WebP format, a type of .riff file
    ico,      // Microsoft icon format
    unknown,  // unidentified image types.
};

image_types get_image_type(const wge::byte_t* data, wge::size_t len) noexcept {
    // source: https://oroboro.com/image-format-magic-bytes/
    if (len < 16) {
        return image_types::unknown;
    }

    // .jpg:  FF D8 FF
    // .png:  89 50 4E 47 0D 0A 1A 0A
    // .gif:  GIF87a
    //        GIF89a
    // .tiff: 49 49 2A 00
    //        4D 4D 00 2A
    // .bmp:  BM
    // .webp: RIFF ???? WEBP
    // .ico   00 00 01 00
    //        00 00 02 00 ( cursor files )

    const auto* str_data = reinterpret_cast<const char*>(data);

    switch (str_data[0]) {
    case '\xFF':
        return (!strncmp(str_data, "\xFF\xD8\xFF", 3)) ? image_types::jpg : image_types::unknown;

    case '\x89':
        return (!strncmp(str_data, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8)) ? image_types::png : image_types::unknown;

    case 'G':
        return (!strncmp(str_data, "GIF87a", 6) || !strncmp(str_data, "GIF89a", 6)) ? image_types::gif
                                                                                    : image_types::unknown;

    case 'I':
        return (!strncmp(str_data, "\x49\x49\x2A\x00", 4)) ? image_types::tiff : image_types::unknown;

    case 'M':
        return (!strncmp(str_data, "\x4D\x4D\x00\x2A", 4)) ? image_types::tiff : image_types::unknown;

    case 'B':
        return ((str_data[1] == 'M')) ? image_types::bmp : image_types::unknown;

    case 'R':
        if (strncmp(str_data, "RIFF", 4)) return image_types::unknown;
        if (strncmp((str_data + 8), "WEBP", 4)) return image_types::unknown;
        return image_types::webp;

    case '\0':
        if (!strncmp(str_data, "\x00\x00\x01\x00", 4)) return image_types::ico;
        if (!strncmp(str_data, "\x00\x00\x02\x00", 4)) return image_types::ico;
        return image_types::unknown;

    default:
        return image_types::unknown;
    }
}
}  // namespace

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
    const auto img_type = ::get_image_type(buffer, buffer_size);
    if (img_type == image_types::jpg) {
        return load_jpeg(buffer, buffer_size);
    }

    if (img_type == image_types::png) {
        return load_png(buffer, buffer_size);
    }

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

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <jpeglib.h>

#include <wge/memory.hpp>
#include <wge/math/utils.hpp>

namespace wge {
namespace video {

static void jpg_null([[maybe_unused]] j_decompress_ptr cinfo) {}

static boolean jpg_fill_input_buffer([[maybe_unused]] j_decompress_ptr cinfo) {
    ////    ri.Con_Printf(PRINT_ALL, "Premature end of JPEG data\n");
    return 1;
}

static void jpg_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
    cinfo->src->next_input_byte += (size_t)num_bytes;
    cinfo->src->bytes_in_buffer -= (size_t)num_bytes;

    //// if (cinfo->src->bytes_in_buffer < 0)
    ////		ri.Con_Printf(PRINT_ALL, "Premature end of JPEG data\n");
}

texture_data image_loader::load_jpeg(const wge::byte_t* const buffer, wge::size_t buffer_size) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    wge::byte_t *p, *q;
    int i;

    // Initialize libJpeg Object
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    cinfo.src = (struct jpeg_source_mgr*)(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT,
                                                                   sizeof(struct jpeg_source_mgr));
    cinfo.src->init_source = jpg_null;
    cinfo.src->fill_input_buffer = jpg_fill_input_buffer;
    cinfo.src->skip_input_data = jpg_skip_input_data;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->term_source = jpg_null;
    cinfo.src->bytes_in_buffer = buffer_size;
    cinfo.src->next_input_byte = buffer;

    // Process JPEG header
    jpeg_read_header(&cinfo, true);

    // Start Decompression
    jpeg_start_decompress(&cinfo);

    // Check Colour Components
    if (cinfo.output_components != 3 && cinfo.output_components != 4) {
        ////		ri.Con_Printf(PRINT_ALL, "Invalid JPEG colour components\n");
        jpeg_destroy_decompress(&cinfo);
        ////		ri.FS_FreeFile(rawdata);
        return {};
    }

    const wge::size_t tw = wge::math::nearest_superior_power_of_2(cinfo.output_width);
    const wge::size_t th = wge::math::nearest_superior_power_of_2(cinfo.output_height);

    constexpr wge::size_t pixel_size = 4U;
    const wge::size_t rgba_data_size = tw * th * pixel_size;
    auto rgba_data = std::make_unique<wge::byte_t[]>(rgba_data_size);

    {
        // Allocate Scanline buffer
        auto scanline = std::make_unique<wge::byte_t[]>(cinfo.output_width * 3);

        // Read Scanlines, and expand from RGB to RGBA
        wge::byte_t* currRow = rgba_data.get();
        while (cinfo.output_scanline < cinfo.output_height) {
            p = scanline.get();
            jpeg_read_scanlines(&cinfo, &p, 1);

            q = currRow;
            for (i = 0; i < (int)cinfo.output_width; i++) {
                q[0] = p[0];
                q[1] = p[1];
                q[2] = p[2];
                q[3] = 255;

                p += 3;
                q += 4;
            }
            currRow += tw * 4;
        }
    }

    texture_data data{cinfo.output_width, cinfo.output_height, tw, th, rgba_data.release(), 4};

    jpeg_finish_decompress(&cinfo);
    // Destroy JPEG object
    jpeg_destroy_decompress(&cinfo);

    return data;
}

}  // namespace video
}  // namespace wge

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <png.h>

#include <wge/memory.hpp>
#include <wge/math/utils.hpp>

#include <algorithm>

namespace wge {
namespace video {
static void PNGCustomWarningFn([[maybe_unused]] png_structp png_ptr, [[maybe_unused]] png_const_charp warning_msg) {
    // ignore PNG warnings
}

struct stream_wrapper {
    stream_wrapper(const wge::byte_t* buf, wge::size_t size) noexcept : length(size), buffer(buf) {}

    wge::size_t read(wge::byte_t* in_buf, wge::size_t size) noexcept {
        size = std::min(size, (length - offset));
        std::copy_n(buffer + offset, size, in_buf);
        offset += size;
        return size;
    }

private:
    wge::size_t offset = 0U;
    wge::size_t length = 0U;
    const wge::byte_t* buffer = nullptr;
};

static void png_read_from_stream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == nullptr) return;  // add custom error handling here

    // using pulsar::InputStream
    // -> replace with your own data source interface
    auto& input_stream = *(stream_wrapper*)io_ptr;
    const auto bytesRead = input_stream.read(outBytes, byteCountToRead);

    if ((png_size_t)bytesRead != byteCountToRead) {
        png_error(png_ptr, "Read Error!");
    }
}

texture_data image_loader::load_png(const wge::byte_t* const buffer, wge::size_t buffer_size) {
    unsigned int sig_read = 0;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, x, y;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        return {};
    }

    png_set_error_fn(png_ptr, nullptr, nullptr, PNGCustomWarningFn);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        return {};
    }
    png_init_io(png_ptr, NULL);

    stream_wrapper input_stream(buffer, buffer_size);
    png_set_read_fn(png_ptr, &input_stream, png_read_from_stream);

    png_set_sig_bytes(png_ptr, sig_read);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, nullptr, nullptr);
    png_set_strip_16(png_ptr);
    png_set_packing(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

    auto* line = reinterpret_cast<wge::u32*>(malloc(width * 4));
    if (!line) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return {};
    }

    const wge::size_t tw = wge::math::nearest_superior_power_of_2(width);
    const wge::size_t th = wge::math::nearest_superior_power_of_2(height);

    constexpr wge::size_t pixel_size = 4U;
    const wge::size_t rgba_data_size = tw * th * pixel_size;
    auto rgba_data = std::make_unique<wge::byte_t[]>(rgba_data_size);

    wge::u32* p32 = nullptr;
    if (rgba_data) {
        p32 = reinterpret_cast<wge::u32*>(rgba_data.get());

        for (y = 0; y < (int)height; y++) {
            png_read_row(png_ptr, reinterpret_cast<wge::byte_t*>(line), nullptr);
            for (x = 0; x < (int)width; x++) {
                wge::u32 color32 = line[x];
                int a = (color32 >> 24) & 0xff;
                int r = color32 & 0xff;
                int g = (color32 >> 8) & 0xff;
                int b = (color32 >> 16) & 0xff;

                color32 = r | (g << 8) | (b << 16) | (a << 24);
                *(p32 + x) = color32;
            }
            p32 += tw;
        }
    }

    free(line);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    texture_data data{width, height, tw, th, rgba_data.release(), 4};
    return data;
}
}  // namespace video
}  // namespace wge
