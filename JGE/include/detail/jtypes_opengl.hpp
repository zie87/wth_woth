//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef JGE_DETAIL_JTYPESOPENGL_HPP
#define JGE_DETAIL_JTYPESOPENGL_HPP

#include <wge/types.hpp>

#if (defined WIN32) && (!defined LINUX)
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <stdint.h>

#ifndef WIN32
#define DEFAULT_BLEND GU_TFX_MODULATE
#define BLEND_OPTION_ADD GU_TFX_ADD
#define BLEND_OPTION_BLEND GU_TFX_BLEND
#endif


#if defined(LINUX)
typedef uint32_t DWORD;
typedef bool BOOL;
#endif

#if (defined FORCE_GLES)
#undef GL_ES_VERSION_2_0
#undef GL_VERSION_2_0
#define GL_VERSION_ES_CM_1_1 1
#ifndef GL_OES_VERSION_1_1
#define glOrthof glOrtho
#define glClearDepthf glClearDepth
#endif
#endif

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define BLEND_ZERO GL_ZERO
#define BLEND_ONE GL_ONE
#define BLEND_SRC_COLOR GL_SRC_COLOR
#define BLEND_ONE_MINUS_SRC_COLOR GL_ONE_MINUS_SRC_COLOR
#define BLEND_SRC_ALPHA GL_SRC_ALPHA
#define BLEND_ONE_MINUS_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
#define BLEND_DST_ALPHA GL_DST_ALPHA
#define BLEND_ONE_MINUS_DST_ALPHA GL_ONE_MINUS_DST_ALPHA
#define BLEND_DST_COLOR GL_DST_COLOR
#define BLEND_ONE_MINUS_DST_COLOR GL_ONE_MINUS_DST_COLOR
#define BLEND_SRC_ALPHA_SATURATE GL_SRC_ALPHA_SATURATE

#define ARGB(a, r, g, b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define RGBA(r, g, b, a) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r))

#define TEXTURE_FORMAT 0
#define GU_PSM_8888 0
#define GU_PSM_5551 0
#define GU_PSM_4444 0
#define GU_PSM_5650 0
#define PIXEL_TYPE wge::pixel_t

#endif
