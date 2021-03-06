#ifndef WGE_VIDEO_UTILS_HPP
#define WGE_VIDEO_UTILS_HPP

#include <wge/types.hpp>
#include <wge/utils/warnings.hpp>

namespace wge {
namespace video {

WGE_DISABLE_WARNING_PUSH
WGE_DISABLE_WARNING_CAST_ALIGN

struct utils final {
    static void swizzle_lines(const u8* inSrc, u8* inDst, unsigned int inWidth, unsigned int inLines) {
        const auto rowblocks = (inWidth * sizeof(u32) / 16);
        for (wge::size_t j = 0; j < inLines; ++j) {
            for (wge::size_t i = 0; i < inWidth * sizeof(u32); ++i) {
                const auto blockx = i / 16;
                const auto blocky = j / 8;

                const auto x = (i - blockx * 16);
                const auto y = (j - blocky * 8);
                const auto block_index = blockx + ((blocky)*rowblocks);
                const auto block_address = block_index * 16 * 8;

                inDst[block_address + x + y * 16] = inSrc[i + j * inWidth * sizeof(u32)];
            }
        }
    }

    static inline void swizzle_row(const wge::byte_t* inSrc, wge::u32*& inDst, wge::size_t block_width,
                                   wge::size_t inPitch) noexcept {
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

    static inline void swizzle_fast(wge::byte_t* out, const wge::byte_t* in, wge::size_t width,
                                    wge::size_t height) noexcept {
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

WGE_DISABLE_WARNING_POP

}  // namespace video
}  // namespace wge

#endif  // WGE_VIDEO_UTILS_HPP
