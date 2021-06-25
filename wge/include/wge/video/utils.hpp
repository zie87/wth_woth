#ifndef WGE_VIDEO_UTILS_HPP
#define WGE_VIDEO_UTILS_HPP

#include <wge/types.hpp>

namespace wge {
namespace video {

struct utils final {
    static void swizzle_lines(const u8* inSrc, u8* inDst, unsigned int inWidth, unsigned int inLines) {
        unsigned int rowblocks = (inWidth * sizeof(u32) / 16);
        for (unsigned int j = 0; j < inLines; ++j) {
            for (unsigned int i = 0; i < inWidth * sizeof(u32); ++i) {
                unsigned int blockx = i / 16;
                unsigned int blocky = j / 8;

                unsigned int x = (i - blockx * 16);
                unsigned int y = (j - blocky * 8);
                unsigned int block_index = blockx + ((blocky)*rowblocks);
                unsigned int block_address = block_index * 16 * 8;

                inDst[block_address + x + y * 16] = inSrc[i + j * inWidth * sizeof(u32)];
            }
        }
    }

    static inline void swizzle_row(const wge::byte_t* inSrc, wge::u32*& inDst, wge::size_t block_width, wge::size_t inPitch) noexcept {
        for (unsigned int block = 0; block < block_width; ++block) {
            const auto* src = reinterpret_cast<const u32*>(inSrc);
            for (wge::size_t j = 0; j < 8; ++j) {
                *(inDst++) = *(src++);
                *(inDst++) = *(src++);
                *(inDst++) = *(src++);
                *(inDst++) = *(src++);
                src += inPitch;
            }
            inSrc += 16;
        }
    }

    static inline void swizzle_fast(wge::byte_t* out, const wge::byte_t* in, wge::size_t width, wge::size_t height) noexcept {
        const wge::size_t width_blocks = (width / 16);
        const wge::size_t height_blocks = (height / 8);

        const wge::size_t src_pitch = (width - 16) / 4;
        const wge::size_t src_row = width * 8;

        const u8* src = in;
        auto* dst = reinterpret_cast<u32*>(out);

        for (wge::size_t blocky = 0; blocky < height_blocks; ++blocky) {
            swizzle_row(src, dst, width_blocks, src_pitch);
            src += src_row;
        }
    }
};

}  // namespace video
}  // namespace wge

#endif  // WGE_VIDEO_UTILS_HPP
