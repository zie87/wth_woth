#include <wge/video/image_loader.hpp>

#include <wtest/wtest.hpp>

#include <memory>
#include <fstream>
#include <vector>

WTEST_SUITE(WgeVideoImageLoader);

WTEST_SETUP(WgeVideoImageLoader) {}
WTEST_TEAR_DOWN(WgeVideoImageLoader) {}

std::vector<wge::byte_t> read_file_to_buffer(const char* filename) {
    std::ifstream in_file;
    in_file.open(filename, std::ios::in | std::ios::binary | std::ios::ate);

    in_file.seekg(0, std::ios::end);
    const auto size = in_file.tellg();
    in_file.seekg(0, std::ios::beg);

    std::vector<wge::byte_t> buffer{};
    buffer.resize(size);

    in_file.read(reinterpret_cast<char*>(buffer.data()), size);
    in_file.close();

    return buffer;
}

void write_file_From_buffer(const char* filename, wge::video::texture_data& data) {
    std::ofstream ofp(filename, std::ios::out | std::ios::binary);

    const wge::size_t out_size = data.texture_width * data.texture_height * data.channels;
    const auto* out = data.pixels.get();

    ofp.write(reinterpret_cast<const char*>(out), out_size);
    ofp.close();
}

WTEST_CASE(WgeVideoImageLoader, load_png_image_from_buffer) {
    const char* solid_bg_path = TEST_DATA_DIR "/bg_solid_colors.png";

    auto buffer = read_file_to_buffer(solid_bg_path);
    const auto data = wge::video::image_loader::load_image(buffer.data(), buffer.size());

    constexpr wge::size_t img_width = 256;
    constexpr wge::size_t img_height = 128;
    constexpr wge::size_t pixel_size = 4;

    WTEST_ASSERT_EQUAL(img_width, data.width);
    WTEST_ASSERT_EQUAL(img_height, data.height);

    WTEST_ASSERT_EQUAL(img_width, data.texture_width);
    WTEST_ASSERT_EQUAL(img_height, data.texture_height);

    WTEST_ASSERT_EQUAL(pixel_size, data.channels);

    WTEST_ASSERT(data.pixels);

    const auto expected = read_file_to_buffer(TEST_DATA_DIR "/bg_solid_colors_png.dat");

    auto* memory_ptr = data.pixels.get();
    for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
        WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
    }
}

WTEST_CASE(WgeVideoImageLoader, load_png_image_from_stream) {
    const char* solid_bg_path = TEST_DATA_DIR "/bg_solid_colors.png";

    std::ifstream stream(solid_bg_path, std::ios::in);
    const auto data = wge::video::image_loader::load_image(stream);

    constexpr wge::size_t img_width = 256;
    constexpr wge::size_t img_height = 128;
    constexpr wge::size_t pixel_size = 4;

    WTEST_ASSERT_EQUAL(img_width, data.width);
    WTEST_ASSERT_EQUAL(img_height, data.height);

    WTEST_ASSERT_EQUAL(img_width, data.texture_width);
    WTEST_ASSERT_EQUAL(img_height, data.texture_height);

    WTEST_ASSERT_EQUAL(pixel_size, data.channels);

    WTEST_ASSERT(data.pixels);

    const auto expected = read_file_to_buffer(TEST_DATA_DIR "/bg_solid_colors_png.dat");

    auto* memory_ptr = data.pixels.get();
    for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
        WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
    }
}

WTEST_CASE(WgeVideoImageLoader, load_jpg_image_from_buffer) {
    {
        const char* solid_bg_path = TEST_DATA_DIR "/bg_solid_colors.jpg";
        auto buffer = read_file_to_buffer(solid_bg_path);
        const auto data = wge::video::image_loader::load_image(buffer.data(), buffer.size());

        constexpr wge::size_t img_width = 256;
        constexpr wge::size_t img_height = 128;
        constexpr wge::size_t pixel_size = 4;

        WTEST_ASSERT_EQUAL(img_width, data.width);
        WTEST_ASSERT_EQUAL(img_height, data.height);

        WTEST_ASSERT_EQUAL(img_width, data.texture_width);
        WTEST_ASSERT_EQUAL(img_height, data.texture_height);

        WTEST_ASSERT_EQUAL(pixel_size, data.channels);

        WTEST_ASSERT(data.pixels);

        const auto expected = read_file_to_buffer(TEST_DATA_DIR "/bg_solid_colors_jpg.dat");

        auto* memory_ptr = data.pixels.get();
        for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
        }
    }

    {
        const char* back_file = TEST_DATA_DIR "/back.jpg";
        auto buffer = read_file_to_buffer(back_file);
        const auto data = wge::video::image_loader::load_image(buffer.data(), buffer.size());

        WTEST_ASSERT(data.pixels);
        constexpr wge::size_t img_width = 200;
        constexpr wge::size_t img_height = 285;
        constexpr wge::size_t tex_width = 256;
        constexpr wge::size_t tex_height = 512;
        constexpr wge::size_t pixel_size = 4;

        WTEST_ASSERT_EQUAL(img_width, data.width);
        WTEST_ASSERT_EQUAL(img_height, data.height);

        WTEST_ASSERT_EQUAL(tex_width, data.texture_width);
        WTEST_ASSERT_EQUAL(tex_height, data.texture_height);

        WTEST_ASSERT_EQUAL(pixel_size, data.channels);

        const auto expected = read_file_to_buffer(TEST_DATA_DIR "/back_jpg.dat");

        auto* memory_ptr = data.pixels.get();
        for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
        }
    }

    {
        const char* back_file = TEST_DATA_DIR "/elsa.jpg";
        auto buffer = read_file_to_buffer(back_file);
        const auto data = wge::video::image_loader::load_image(buffer.data(), buffer.size());

        WTEST_ASSERT(data.pixels);
        constexpr wge::size_t img_width = 480;
        constexpr wge::size_t img_height = 272;
        constexpr wge::size_t tex_width = 512;
        constexpr wge::size_t tex_height = 512;
        constexpr wge::size_t pixel_size = 4;

        WTEST_ASSERT_EQUAL(img_width, data.width);
        WTEST_ASSERT_EQUAL(img_height, data.height);

        WTEST_ASSERT_EQUAL(tex_width, data.texture_width);
        WTEST_ASSERT_EQUAL(tex_height, data.texture_height);

        WTEST_ASSERT_EQUAL(pixel_size, data.channels);

        const auto expected = read_file_to_buffer(TEST_DATA_DIR "/elsa_jpg.dat");

        auto* memory_ptr = data.pixels.get();
        for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
        }
    }
}

WTEST_CASE(WgeVideoImageLoader, load_jpg_image_from_stream) {
    {
        const char* solid_bg_path = TEST_DATA_DIR "/bg_solid_colors.jpg";

        std::ifstream stream(solid_bg_path, std::ios::in);
        const auto data = wge::video::image_loader::load_image(stream);

        constexpr wge::size_t img_width = 256;
        constexpr wge::size_t img_height = 128;
        constexpr wge::size_t pixel_size = 4;

        WTEST_ASSERT_EQUAL(img_width, data.width);
        WTEST_ASSERT_EQUAL(img_height, data.height);

        WTEST_ASSERT_EQUAL(img_width, data.texture_width);
        WTEST_ASSERT_EQUAL(img_height, data.texture_height);

        WTEST_ASSERT_EQUAL(pixel_size, data.channels);

        WTEST_ASSERT(data.pixels);

        const auto expected = read_file_to_buffer(TEST_DATA_DIR "/bg_solid_colors_jpg.dat");

        auto* memory_ptr = data.pixels.get();
        for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
        }
    }

    {
        const char* back_file = TEST_DATA_DIR "/back.jpg";
        std::ifstream stream(back_file, std::ios::in);
        const auto data = wge::video::image_loader::load_image(stream);

        WTEST_ASSERT(data.pixels);
        constexpr wge::size_t img_width = 200;
        constexpr wge::size_t img_height = 285;
        constexpr wge::size_t tex_width = 256;
        constexpr wge::size_t tex_height = 512;
        constexpr wge::size_t pixel_size = 4;

        WTEST_ASSERT_EQUAL(img_width, data.width);
        WTEST_ASSERT_EQUAL(img_height, data.height);

        WTEST_ASSERT_EQUAL(tex_width, data.texture_width);
        WTEST_ASSERT_EQUAL(tex_height, data.texture_height);

        WTEST_ASSERT_EQUAL(pixel_size, data.channels);

        const auto expected = read_file_to_buffer(TEST_DATA_DIR "/back_jpg.dat");

        auto* memory_ptr = data.pixels.get();
        for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
        }
    }

    {
        const char* back_file = TEST_DATA_DIR "/elsa.jpg";
        std::ifstream stream(back_file, std::ios::in);
        const auto data = wge::video::image_loader::load_image(stream);

        WTEST_ASSERT(data.pixels);
        constexpr wge::size_t img_width = 480;
        constexpr wge::size_t img_height = 272;
        constexpr wge::size_t tex_width = 512;
        constexpr wge::size_t tex_height = 512;
        constexpr wge::size_t pixel_size = 4;

        WTEST_ASSERT_EQUAL(img_width, data.width);
        WTEST_ASSERT_EQUAL(img_height, data.height);

        WTEST_ASSERT_EQUAL(tex_width, data.texture_width);
        WTEST_ASSERT_EQUAL(tex_height, data.texture_height);

        WTEST_ASSERT_EQUAL(pixel_size, data.channels);

        const auto expected = read_file_to_buffer(TEST_DATA_DIR "/elsa_jpg.dat");

        auto* memory_ptr = data.pixels.get();
        for (wge::size_t idx = 0; idx < expected.size(); ++idx) {
            WTEST_ASSERT_EQUAL(expected[idx], memory_ptr[idx]);
        }
    }
}

WTEST_SUITE_RUNNER(WgeVideoImageLoader) {
    RUN_TEST_CASE(WgeVideoImageLoader, load_png_image_from_buffer);
    RUN_TEST_CASE(WgeVideoImageLoader, load_png_image_from_stream);

    RUN_TEST_CASE(WgeVideoImageLoader, load_jpg_image_from_buffer);
    RUN_TEST_CASE(WgeVideoImageLoader, load_jpg_image_from_stream);
}
