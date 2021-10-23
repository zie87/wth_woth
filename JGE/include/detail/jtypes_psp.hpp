//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef JGE_DETAIL_JTYPESPSP_HPP
#define JGE_DETAIL_JTYPESPSP_HPP

#include <wge/types.hpp>

#include <pspgu.h>
#include <pspgum.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspaudiolib.h>
#include <psprtc.h>

#include <time.h>
#include <string.h>

#define DEFAULT_BLEND GU_TFX_MODULATE
#define BLEND_OPTION_ADD GU_TFX_ADD
#define BLEND_OPTION_BLEND GU_TFX_BLEND

#ifndef ABGR8888
#define ABGR8888
#endif

#if defined(ABGR8888)
#define PIXEL_TYPE u32
#ifndef ARGB
#define ARGB(a, r, g, b) \
    (PIXEL_TYPE)((a << 24) | (b << 16) | (g << 8) | r)  // macro to assemble pixels in correct format
#endif
#define MAKE_COLOR(a, c) (a << 24 | c)
#define MASK_ALPHA 0xFF000000  // masks for accessing individual pixels
#define MASK_BLUE 0x00FF0000
#define MASK_GREEN 0x0000FF00
#define MASK_RED 0x000000FF

#define PIXEL_SIZE 4
#define PIXEL_FORMAT PSP_DISPLAY_PIXEL_FORMAT_8888

#define BUFFER_FORMAT GU_PSM_8888
#define TEXTURE_FORMAT GU_PSM_8888
#define TEXTURE_COLOR_FORMAT GU_COLOR_8888

#elif defined(ABGR5551)

#ifndef ARGB
#define ARGB(a, r, g, b) ((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10) | ((a >> 7) << 15))
#endif
#define MAKE_COLOR(a, c) (((a >> 7) << 15) | c)
#define MASK_ALPHA 0x8000
#define MASK_BLUE 0x7C00
#define MASK_GREEN 0x03E0
#define MASK_RED 0x001F
#define PIXEL_TYPE u16
#define PIXEL_SIZE 2
#define PIXEL_FORMAT PSP_DISPLAY_PIXEL_FORMAT_5551

#define BUFFER_FORMAT GU_PSM_8888
#define TEXTURE_FORMAT GU_PSM_5551
#define TEXTURE_COLOR_FORMAT GU_COLOR_5551

#elif defined(ABGR4444)
#ifndef ARGB
#define ARGB(a, r, g, b) ((r >> 4) | ((g >> 4) << 4) | ((b >> 4) << 8) | ((a >> 4) << 12))
#endif
#define MAKE_COLOR(a, c) (((a >> 4) << 12) | c)
#define MASK_ALPHA 0xF000
#define MASK_BLUE 0x0F00
#define MASK_GREEN 0x00F0
#define MASK_RED 0x000F
#define PIXEL_TYPE u16
#define PIXEL_SIZE 2
#define PIXEL_FORMAT PSP_DISPLAY_PIXEL_FORMAT_4444

#define BUFFER_FORMAT GU_PSM_4444
#define TEXTURE_FORMAT GU_PSM_4444
#define TEXTURE_COLOR_FORMAT GU_COLOR_4444

#endif

#define FRAME_BUFFER_WIDTH 512
#define FRAME_BUFFER_SIZE FRAME_BUFFER_WIDTH* SCREEN_HEIGHT* PIXEL_SIZE

#define SLICE_SIZE_F 64.0f
typedef unsigned int DWORD;

#define BLEND_ZERO 0x1000
#define BLEND_ONE 0x1002
#define BLEND_SRC_COLOR GU_SRC_COLOR
#define BLEND_ONE_MINUS_SRC_COLOR GU_ONE_MINUS_SRC_COLOR
#define BLEND_SRC_ALPHA GU_SRC_ALPHA
#define BLEND_ONE_MINUS_SRC_ALPHA GU_ONE_MINUS_SRC_ALPHA
#define BLEND_DST_ALPHA GU_DST_ALPHA
#define BLEND_ONE_MINUS_DST_ALPHA GU_ONE_MINUS_DST_ALPHA
#define BLEND_DST_COLOR GU_DST_COLOR
#define BLEND_ONE_MINUS_DST_COLOR GU_ONE_MINUS_DST_COLOR
#define BLEND_SRC_ALPHA_SATURATE BLEND_ONE

typedef struct {
    ScePspFVector2 texture;
    ScePspFVector3 pos;
} PSPVertex3D;

#endif
