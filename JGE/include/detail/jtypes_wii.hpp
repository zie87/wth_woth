#ifndef JGE_DETAIL_JTYPESWII_HPP
#define JGE_DETAIL_JTYPESWII_HPP

#include <wge/types.hpp>

#include <ogc/gx.h>

// #define DEFAULT_BLEND GU_TFX_MODULATE
// #define BLEND_OPTION_ADD GU_TFX_ADD
// #define BLEND_OPTION_BLEND GU_TFX_BLEND

typedef wge::i32 DWORD;

//typedef int8_t s8;
//typedef int16_t s16;
//typedef int32_t s32;
//typedef uint8_t u8;
//typedef uint16_t u16;
//typedef uint32_t u32;

//#define BLEND_ZERO GL_ZERO
//#define BLEND_ONE GL_ONE
//#define BLEND_SRC_COLOR GL_SRC_COLOR
//#define BLEND_ONE_MINUS_SRC_COLOR GL_ONE_MINUS_SRC_COLOR
//#define BLEND_SRC_ALPHA GL_SRC_ALPHA
//#define BLEND_ONE_MINUS_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
//#define BLEND_DST_ALPHA GL_DST_ALPHA
//#define BLEND_ONE_MINUS_DST_ALPHA GL_ONE_MINUS_DST_ALPHA
//#define BLEND_DST_COLOR GL_DST_COLOR
//#define BLEND_ONE_MINUS_DST_COLOR GL_ONE_MINUS_DST_COLOR
//#define BLEND_SRC_ALPHA_SATURATE GL_SRC_ALPHA_SATURATE

#define ARGB(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define RGBA(r, g, b, a) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r))

#define TEXTURE_FORMAT 0
//#define GU_PSM_8888 0
//#define GU_PSM_5551 0
//#define GU_PSM_4444 0
//#define GU_PSM_5650 0
#define PIXEL_TYPE wge::pixel_t

#endif
