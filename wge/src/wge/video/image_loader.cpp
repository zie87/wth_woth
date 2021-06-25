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

texture_data image_loader::load_image(std::istream& stream, pixel_format format, bool use_vram,
                                      bool swizzle) noexcept {
    const auto pos = stream.tellg();

    constexpr std::size_t min_check_size = 16U;
    wge::byte_t check_buf[min_check_size];

    stream.get(reinterpret_cast<char*>(check_buf), min_check_size);
    const auto img_type = ::get_image_type(check_buf, stream.gcount());
    stream.seekg(pos);

    if (img_type == image_types::jpg) {
        return load_jpeg(stream, format, use_vram, swizzle);
    }

    if (img_type == image_types::png) {
        return load_png(stream, format, use_vram, swizzle);
    }

    printf("*** unkown format\n");
    return {};
}

texture_data image_loader::load_image(const wge::byte_t* const buffer, wge::size_t buffer_size, pixel_format format,
                                      bool use_vram, bool swizzle) noexcept {
    const auto img_type = ::get_image_type(buffer, buffer_size);
    if (img_type == image_types::jpg) {
        return load_jpeg(buffer, buffer_size, format, use_vram, swizzle);
    }

    if (img_type == image_types::png) {
        return load_png(buffer, buffer_size, format, use_vram, swizzle);
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

struct jpeg_stream final {
    jpeg_source_mgr pub;
    std::istream* stream;
    std::array<wge::byte_t, 4096> buffer;
};

void init_source(j_decompress_ptr cinfo) noexcept {
    auto* src = reinterpret_cast<jpeg_stream*>(cinfo->src);
    src->stream->seekg(0, std::ios::beg);
}

boolean fill_buffer(j_decompress_ptr cinfo) noexcept {
    // Read to buffer
    auto* src = reinterpret_cast<jpeg_stream*>(cinfo->src);
    std::istream& stream = *(src->stream);
    stream.get(reinterpret_cast<char*>(src->buffer.data()), src->buffer.size());

    src->pub.next_input_byte = src->buffer.data();
    src->pub.bytes_in_buffer = stream.gcount();

    return stream.eof() ? TRUE : FALSE;
}

void skip(j_decompress_ptr cinfo, long count) noexcept {
    auto* src = reinterpret_cast<jpeg_stream*>(cinfo->src);
    std::istream& stream = *(src->stream);
    stream.seekg(count, std::ios::cur);

    fill_buffer(cinfo);
}

void term(j_decompress_ptr cinfo) noexcept {
    // Close the stream, can be nop
}

void make_jpeg_info(jpeg_decompress_struct& cinfo, const wge::byte_t* const buffer, wge::size_t buffer_size) noexcept {
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

void make_jpeg_info(jpeg_decompress_struct& cinfo, std::istream& is) noexcept {
    /* The source object and input buffer are made permanent so that a series
     * of JPEG images can be read from the same file by calling jpeg_stdio_src
     * only before the first one.  (If we discarded the buffer at the end of
     * one image, we'd likely lose the start of the next one.)
     * This makes it unsafe to use this manager and a different source
     * manager serially with the same JPEG object.  Caveat programmer.
     */
    if (cinfo.src == nullptr) {
        /* first time for this JPEG object? */
        cinfo.src = (struct jpeg_source_mgr*)(*cinfo.mem->alloc_small)((j_common_ptr)&cinfo, JPOOL_PERMANENT,
                                                                       sizeof(jpeg_stream));
    }

    auto* src = reinterpret_cast<jpeg_stream*>(cinfo.src);
    src->stream = &is;
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_buffer;
    src->pub.skip_input_data = skip;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
    src->pub.term_source = term;
    src->pub.bytes_in_buffer = 0;       /* forces fill_input_buffer on first read */
    src->pub.next_input_byte = nullptr; /* until buffer loaded */
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

texture_data read_jpg_data(jpeg_decompress_struct& cinfo, pixel_format format, bool use_vram, bool swizzle) noexcept {
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
    if (!ram_ptr) {
        return {};
    }

    wge::byte_t* work_buf = nullptr;
    if (swizzle) {
        work_buf = new wge::byte_t[size];
    } else {
        work_buf = ram_ptr.get();
    }

    if (!work_buf) {
        return {};
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

    return texture_data{cinfo.output_width, cinfo.output_height, tw, th, std::move(ram_ptr), 4};
}

}  // namespace

texture_data image_loader::load_jpeg(const wge::byte_t* const buffer, wge::size_t buffer_size, pixel_format format,
                                     bool use_vram, bool swizzle) noexcept {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    make_jpeg_info(cinfo, buffer, buffer_size);

    auto data = read_jpg_data(cinfo, format, use_vram, swizzle);

    jpeg_destroy_decompress(&cinfo);

    return data;
}

texture_data image_loader::load_jpeg(std::istream& stream, pixel_format format, bool use_vram, bool swizzle) noexcept {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    make_jpeg_info(cinfo, stream);

    auto data = read_jpg_data(cinfo, format, use_vram, swizzle);

    jpeg_destroy_decompress(&cinfo);

    return data;
}

}  // namespace video
}  // namespace wge

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

#include <png.h>

#include <wge/memory.hpp>
#include <wge/math/utils.hpp>

namespace wge {
namespace video {
static void PNGCustomWarningFn([[maybe_unused]] png_structp png_ptr, [[maybe_unused]] png_const_charp warning_msg) {
    // TODO: log warnings!
}

namespace {

void read_png_line(png_structp& png_ptr, wge::byte_t* scanline, wge::size_t width, pixel_format format,
                   wge::byte_t* buf) noexcept {
    png_read_row(png_ptr, scanline, nullptr);

    switch (format) {
    case pixel_format::format_5650:
        convert_image_line<pixel_format::format_5650>(scanline, width, 4, reinterpret_cast<wge::u16*>(buf));
        break;
    case pixel_format::format_5551:
        convert_image_line<pixel_format::format_5551>(scanline, width, 4, reinterpret_cast<wge::u16*>(buf));
        break;
    case pixel_format::format_4444:
        convert_image_line<pixel_format::format_4444>(scanline, width, 4, reinterpret_cast<wge::u16*>(buf));
        break;
    case pixel_format::format_8888:
        convert_image_line<pixel_format::format_8888>(scanline, width, 4, reinterpret_cast<wge::u32*>(buf));
        break;
    case pixel_format::none:
        convert_image_line<pixel_format::none>(scanline, width, 4, reinterpret_cast<wge::u32*>(buf));
        break;
    }
}

texture_data read_png_data(png_structp& png_ptr, png_infop& info_ptr, bool use_vram, pixel_format format,
                           bool swizzle) noexcept {
    const unsigned int sig_read = 0;
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

    const wge::size_t tw = wge::math::nearest_superior_power_of_2(width);
    const wge::size_t th = wge::math::nearest_superior_power_of_2(height);

    const auto pixel_size = get_pixel_size(format);
    const wge::size_t size = tw * th * pixel_size;
    auto ram_ptr = make_vram_ptr<wge::byte_t>(size, use_vram);
    if (!ram_ptr) {
        return {};
    }

    wge::byte_t* work_buf = nullptr;

    constexpr wge::size_t vertical_block_size = 8U;
    const auto swizzle_size = tw * vertical_block_size * pixel_size;
    if (swizzle) {
        work_buf = new wge::byte_t[swizzle_size];
    } else {
        work_buf = ram_ptr.get();
    }

    if (work_buf == nullptr) {
        return {};
    }

    const auto row_size = tw * pixel_size;
    {
        auto scanline = std::make_unique<wge::byte_t[]>(width * 4);
        if (!scanline) {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);

            if (swizzle && (work_buf != nullptr)) {
                delete[] work_buf;
            }
            return {};
        }

        const auto num_vertical_blocks = height / vertical_block_size;

        wge::byte_t* dst = ram_ptr.get();
        wge::byte_t* tmp_ptr = work_buf;

        for (wge::size_t block = 0; block < num_vertical_blocks; ++block) {
            for (y = 0; y < vertical_block_size; y++) {
                read_png_line(png_ptr, scanline.get(), width, format, tmp_ptr);
                tmp_ptr += row_size;
            }

            if (swizzle) {
                utils::swizzle_fast(dst, work_buf, row_size, vertical_block_size);
                dst += row_size * vertical_block_size;
                tmp_ptr = work_buf;
            }
        }

        // remaining lines (if height was not evenly devisibble by vertical block size)
        if (swizzle) {
            // clear the conversion buffer so that leftover scan lines are transparent
            std::fill_n(work_buf, swizzle_size, 255);
        }
        const auto remaining_lines = height % vertical_block_size;
        for (wge::size_t line = 0; line < remaining_lines; ++line) {
            read_png_line(png_ptr, scanline.get(), width, format, tmp_ptr);
            tmp_ptr += row_size;
        }

        if (swizzle) {
            utils::swizzle_lines(work_buf, dst, tw, remaining_lines);
        }
    }

    return texture_data(width, height, tw, th, std::move(ram_ptr), pixel_size);
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

texture_data image_loader::load_png(const wge::byte_t* const buffer, wge::size_t buffer_size, pixel_format format,
                                    bool use_vram, bool swizzle) noexcept {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return {};
    }

    png_set_error_fn(png_ptr, nullptr, nullptr, PNGCustomWarningFn);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return {};
    }
    png_init_io(png_ptr, nullptr);

    stream_wrapper input_stream(buffer, buffer_size);
    png_set_read_fn(png_ptr, &input_stream, read_png_from_buffer);

    auto texture_data = read_png_data(png_ptr, info_ptr, use_vram, format, swizzle);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return texture_data;
}

texture_data image_loader::load_png(std::istream& stream, pixel_format format, bool use_vram, bool swizzle) noexcept {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return {};
    }

    png_set_error_fn(png_ptr, nullptr, nullptr, PNGCustomWarningFn);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return {};
    }
    png_init_io(png_ptr, nullptr);

    png_set_read_fn(png_ptr, &stream, read_png_from_stream);

    auto texture_data = read_png_data(png_ptr, info_ptr, use_vram, format, swizzle);

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return texture_data;
}

}  // namespace video
}  // namespace wge
