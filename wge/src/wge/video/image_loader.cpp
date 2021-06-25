#include <wge/video/image_loader.hpp>
#include <wge/video/utils.hpp>

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

texture_data::texture_data(wge::size_t w, wge::size_t h, wge::size_t tw, wge::size_t th, vram_ptr<wge::byte_t>&& buf,
                           wge::size_t channels) noexcept
    : width(w), height(h), texture_width(tw), texture_height(th), pixels(std::move(buf)), channels(channels) {}

// texture_data image_loader::load_image(std::istream& stream) noexcept {
//    const auto pos = stream.tellg();
//
//    constexpr std::size_t min_check_size = 16U;
//    wge::byte_t check_buf[min_check_size];
//
//    stream.get(reinterpret_cast<char*>(check_buf), min_check_size);
//    const auto img_type = ::get_image_type(check_buf, min_check_size);
//    stream.seekg(pos);
//
//    if (img_type == image_types::jpg) {
//        return load_jpeg(stream);
//    }
//
//    if (img_type == image_types::png) {
//        return load_png(stream);
//    }
//
//    return {};
//}

texture_data image_loader::load_image(const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept {
    const auto img_type = ::get_image_type(buffer, buffer_size);
    if (img_type == image_types::jpg) {
        return load_jpeg(buffer, buffer_size);
    }

    if (img_type == image_types::png) {
        return load_png(buffer, buffer_size);
    }

    return {};
}

}  // namespace video
}  // namespace wge

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>

namespace {
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
}  // namespace

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <jpeglib.h>

#include <wge/memory.hpp>
#include <wge/math/utils.hpp>

namespace wge {
namespace video {

namespace {
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

template <wge::video::pixel_format Format>
inline void convert_image_line(wge::byte_t* line, wge::size_t width, wge::size_t num_channels,
                               typename wge::video::pixel_converter<Format>::pixel_t* out) noexcept {
    using Converter = wge::video::pixel_converter<Format>;
    for (wge::size_t x = 0; x < width; ++x) {
        int r = line[0];
        int g = line[1];
        int b = line[2];
        int a = (num_channels == 4) ? line[3] : 255;

        *(out + x) = Converter::convert(r, g, b, a);
        line += num_channels;
    }
}

void fill_jpeg_info(jpeg_decompress_struct& cinfo, const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept {
    cinfo.src = (struct jpeg_source_mgr*)(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT,
                                                                   sizeof(struct jpeg_source_mgr));
    cinfo.src->init_source = jpg_null;
    cinfo.src->fill_input_buffer = jpg_fill_input_buffer;
    cinfo.src->skip_input_data = jpg_skip_input_data;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->term_source = jpg_null;
    cinfo.src->bytes_in_buffer = buffer_size;
    cinfo.src->next_input_byte = buffer;
}

void read_jpg_line(jpeg_decompress_struct& cinfo, wge::byte_t* scanline, pixel_format format,
                   wge::byte_t* buf) noexcept {
    jpeg_read_scanlines(&cinfo, &scanline, 1);
    const auto width = cinfo.output_width;

    switch (format) {
    case pixel_format::format_5650:
        convert_image_line<pixel_format::format_5650>(scanline, width, 3, reinterpret_cast<wge::u16*>(buf));
        break;
    case pixel_format::format_5551:
        convert_image_line<pixel_format::format_5551>(scanline, width, 3, reinterpret_cast<wge::u16*>(buf));
        break;
    case pixel_format::format_4444:
        convert_image_line<pixel_format::format_4444>(scanline, width, 3, reinterpret_cast<wge::u16*>(buf));
        break;
    case pixel_format::format_8888:
        convert_image_line<pixel_format::format_8888>(scanline, width, 3, reinterpret_cast<wge::u32*>(buf));
        break;
    case pixel_format::none:
        convert_image_line<pixel_format::none>(scanline, width, 3, reinterpret_cast<wge::u32*>(buf));
        break;
    }
}
}  // namespace

texture_data image_loader::load_jpeg(const wge::byte_t* const buffer, wge::size_t buffer_size, pixel_format format,
                                     bool use_vram, bool swizzle) noexcept {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    fill_jpeg_info(cinfo, buffer, buffer_size);

    jpeg_read_header(&cinfo, true);
    jpeg_start_decompress(&cinfo);

    if (cinfo.output_components != 3 && cinfo.output_components != 4) {
        jpeg_destroy_decompress(&cinfo);
        return {};
    }

    const auto pixel_size = get_pixel_size(format);
    const wge::size_t tw = wge::math::nearest_superior_power_of_2(cinfo.output_width);
    const wge::size_t th = wge::math::nearest_superior_power_of_2(cinfo.output_height);
    const auto size = tw * th * pixel_size;

    auto ram_ptr = wge::video::make_vram_ptr<wge::byte_t>(size, use_vram);

    wge::byte_t* work_buf = nullptr;
    if (swizzle) {
        work_buf = new wge::byte_t[size];
    } else {
        work_buf = ram_ptr.get();
    }

    const auto row_size = tw * pixel_size;
    {
        auto scanline = std::make_unique<wge::byte_t[]>(cinfo.output_width * 3);
        if (!scanline) {
            jpeg_destroy_decompress(&cinfo);

            if (swizzle && (work_buf != nullptr)) {
                delete[] work_buf;
            }
            return {};
        }

        wge::byte_t* row_ptr = work_buf;
        while (cinfo.output_scanline < cinfo.output_height) {
            read_jpg_line(cinfo, scanline.get(), format, row_ptr);
            row_ptr += row_size;
        }
    }

    jpeg_finish_decompress(&cinfo);

    if (swizzle) {
        auto* buf = reinterpret_cast<wge::u8*>(ram_ptr.get());
        if (work_buf) {
            utils::swizzle_fast(buf, work_buf, row_size, th);
            delete[] work_buf;
        }
    }

    texture_data data{cinfo.output_width, cinfo.output_height, tw, th, std::move(ram_ptr), 4};
    jpeg_destroy_decompress(&cinfo);

    return data;
}

}  // namespace video
}  // namespace wge

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <png.h>

#include <wge/memory.hpp>
#include <wge/math/utils.hpp>

namespace wge {
namespace video {
static void PNGCustomWarningFn([[maybe_unused]] png_structp png_ptr, [[maybe_unused]] png_const_charp warning_msg) {
    // ignore PNG warnings
}

namespace {

texture_data read_png_data(png_structp& png_ptr, png_infop& info_ptr) noexcept {
    unsigned int sig_read = 0;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, x, y;

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
    auto rgba_data = make_vram_ptr<wge::byte_t>(rgba_data_size, false);

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

    return texture_data(width, height, tw, th, std::move(rgba_data), 4);
}

}  // namespace

static void read_png_from_buffer(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == nullptr) return;  // add custom error handling here

    auto& input_stream = *(stream_wrapper*)io_ptr;
    const auto bytesRead = input_stream.read(outBytes, byteCountToRead);

    if ((png_size_t)bytesRead != byteCountToRead) {
        png_error(png_ptr, "Read Error!");
    }
}

static void read_png_from_stream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == nullptr) return;  // add custom error handling here

    auto& input_stream = *(std::istream*)io_ptr;
    input_stream.get(reinterpret_cast<char*>(outBytes), byteCountToRead);

    const auto bytesRead = input_stream.gcount();
    if ((png_size_t)bytesRead != byteCountToRead) {
        png_error(png_ptr, "Read Error!");
    }
}

texture_data image_loader::load_png(const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept {
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
    png_set_read_fn(png_ptr, &input_stream, read_png_from_buffer);

    auto texture_data = read_png_data(png_ptr, info_ptr);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return texture_data;
}

texture_data image_loader::load_png(std::istream& stream) noexcept {
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

    png_set_read_fn(png_ptr, &stream, read_png_from_stream);

    auto texture_data = read_png_data(png_ptr, info_ptr);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return texture_data;
}

}  // namespace video
}  // namespace wge
