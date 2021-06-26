//-------------------------------------------------------------------------------------
//
// JGE is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#include <malloc.h>
#include <pspgu.h>
#include <pspgum.h>
#include <fastmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>

#include <sstream>

#ifdef __cplusplus
extern "C" {
#endif

#include <png.h>

#ifdef __cplusplus
}
#endif

#include "JGE.h"
#include "JRenderer.h"
#include "JFileSystem.h"

#include <wge/math.hpp>
#include <wge/video/pixel_format.hpp>
#include <wge/video/image_loader.hpp>
#include <wge/video/utils.hpp>

#include <wge/log.hpp>

#include <utility>

static unsigned int __attribute__((aligned(16))) list[262144];

extern void SwizzlePlot(u8* out, PIXEL_TYPE color, int i, int j, unsigned int width);

JQuad::JQuad(JTexture* tex, float x, float y, float width, float height)
    : mTex(tex), mX(x), mY(y), mWidth(width), mHeight(height) {
    mHotSpotX = 0.0f;
    mHotSpotY = 0.0f;
    mBlend = DEFAULT_BLEND;  // GU_TFX_MODULATE;
    for (int i = 0; i < 4; i++) mColor[i] = ARGB(255, 255, 255, 255);

    mHFlipped = false;
    mVFlipped = false;
}

void JQuad::GetTextureRect(float* x, float* y, float* w, float* h) {
    *x = mX;
    *y = mY;
    *w = mWidth;
    *h = mHeight;
}

void JQuad::SetTextureRect(float x, float y, float w, float h) {
    mX = x;
    mY = y;
    mWidth = w;
    mHeight = h;
}

void JQuad::SetColor(PIXEL_TYPE color) {
    for (int i = 0; i < 4; i++) mColor[i] = color;
}

void JQuad::SetHotSpot(float x, float y) {
    mHotSpotX = x;
    mHotSpotY = y;
}

//////////////////////////////////////////////////////////////////////////

JTexture::JTexture() {
    mBits = NULL;
    mInVideoRAM = false;
    mTextureFormat = TEXTURE_FORMAT;
}

JTexture::~JTexture() {
    if (mBits) {
        if (mInVideoRAM)
            vfree(mBits);
        else
            free(mBits);
    }
}

void JTexture::UpdateBits(int x, int y, int width, int height, PIXEL_TYPE* bits) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            SwizzlePlot((u8*)mBits, *(bits++), (x + j) * PIXEL_SIZE, y + i, mTexWidth * PIXEL_SIZE);
        }
    }
    sceKernelDcacheWritebackAll();
}

//////////////////////////////////////////////////////////////////////////

JRenderer* JRenderer::mInstance = NULL;

bool JRenderer::m3DEnabled = false;

void JRenderer::Set3DFlag(bool flag) { m3DEnabled = flag; }

JRenderer* JRenderer::GetInstance() {
    if (mInstance == NULL) {
        mInstance = new JRenderer();
        mInstance->InitRenderer();
    }

    return mInstance;
}

void JRenderer::Destroy() {
    if (mInstance) {
        mInstance->DestroyRenderer();
        delete mInstance;
        mInstance = NULL;
    }
}

JRenderer::JRenderer() {}

JRenderer::~JRenderer() {}

void JRenderer::InitRenderer() {
    mCurrentRenderMode = MODE_2D;

#ifdef USING_MATH_TABLE
    for (int i = 0; i < 360; i++) {
        mSinTable[i] = sinf(i * DEG2RAD);
        mCosTable[i] = cosf(i * DEG2RAD);
    }
#endif

    mCurrTexBlendSrc = BLEND_SRC_ALPHA;
    mCurrTexBlendDest = BLEND_ONE_MINUS_SRC_ALPHA;

    mSwizzle = 1;
    mVsync = false;

    mTexCounter = 0;
    mCurrentTex = -1;
    mCurrentBlend = -1;
    mCurrentTextureFormat = TEXTURE_FORMAT;

    mFOV = 75.0f;

    mImageFilter = NULL;

    mCurrentTextureFilter = TEX_FILTER_LINEAR;

    sceGuInit();

    fbp0 = (u32*)valloc(FRAME_BUFFER_SIZE);
    fbp1 = (u32*)valloc(FRAME_BUFFER_SIZE);
    zbp = NULL;

    // setup GU
    sceGuStart(GU_DIRECT, list);

    sceGuDrawBuffer(BUFFER_FORMAT, vrelptr(fbp0), FRAME_BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, vrelptr(fbp1), FRAME_BUFFER_WIDTH);
    if (m3DEnabled) {
        zbp = (u16*)valloc(FRAME_BUFFER_WIDTH * SCREEN_HEIGHT * 2);
        sceGuDepthBuffer(vrelptr(zbp), FRAME_BUFFER_WIDTH);
    }

    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    // sceGuFrontFace(GU_CW);
    sceGuFrontFace(GU_CCW);
    sceGuEnable(GU_TEXTURE_2D);

    sceGuShadeModel(GU_SMOOTH);

    sceGuTexWrap(GU_REPEAT, GU_REPEAT);

    // enable alpha channel
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

    sceGuTexFilter(GU_LINEAR, GU_LINEAR);

    if (m3DEnabled) {
        sceGuDepthRange(65535, 0);
        sceGuEnable(GU_DEPTH_TEST);
        sceGuDepthFunc(GU_GEQUAL);

        sceGuEnable(GU_CULL_FACE);
        sceGuEnable(GU_CLIP_PLANES);

        sceGuClearColor(0x00ff0000);
        sceGuClearDepth(0);

        sceGuTexEnvColor(0xffffffff);

        sceGuTexScale(1.0f, 1.0f);
        sceGuTexOffset(0.0f, 0.0f);
        sceGuAmbientColor(0xffffffff);

        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(mFOV, 16.0f / 9.0f, 0.5f, 1000.0f);
        // sceGumPerspective(90.0f,480.0f/272.0f,0.5f,1000.0f);
    }

    // sceGuClear(GU_COLOR_BUFFER_BIT);
    sceGuFinish();
    sceGuSync(0, 0);

    sceDisplayWaitVblankStart();
    sceGuDisplay(1);
}

void JRenderer::SetTexBlend(int src, int dest) {
    if (src != mCurrTexBlendSrc || dest != mCurrTexBlendDest) {
        mCurrTexBlendSrc = src;
        mCurrTexBlendDest = dest;

        int fixSrc = 0;
        int fixDest = 0;
        if (src == BLEND_ZERO)
            src = GU_FIX;
        else if (src == BLEND_ONE) {
            src = GU_FIX;
            fixSrc = 0x00FFFFFF;
        }
        if (dest == BLEND_ZERO)
            dest = GU_FIX;
        else if (dest == BLEND_ONE) {
            dest = GU_FIX;
            fixDest = 0x00FFFFFF;
        }

        // glBlendFunc(src, dest);
        sceGuBlendFunc(GU_ADD, src, dest, fixSrc, fixDest);
    }
}

void JRenderer::EnableTextureFilter(bool flag) {
    if (flag)
        mCurrentTextureFilter = TEX_FILTER_LINEAR;
    else
        mCurrentTextureFilter = TEX_FILTER_NEAREST;
}

void JRenderer::DestroyRenderer() {
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
    vfree(fbp0);
    vfree(fbp1);
    // debugged = 0;
    if (zbp) vfree(zbp);
}

void JRenderer::BeginScene() {
    sceGuStart(GU_DIRECT, list);

    if (m3DEnabled) {
        // if (mMode3D)
        sceGuClear(GU_DEPTH_BUFFER_BIT | GU_COLOR_BUFFER_BIT);
    } else {
        sceGuClear(GU_COLOR_BUFFER_BIT);
    }

    sceGuTexMode(TEXTURE_FORMAT, 0, 0, mSwizzle);
    mCurrentTextureFormat = TEXTURE_FORMAT;

    if (mCurrentTextureFilter == TEX_FILTER_NEAREST)
        sceGuTexFilter(GU_NEAREST, GU_NEAREST);  // GU_NEAREST good for tile-map
    else
        sceGuTexFilter(GU_LINEAR, GU_LINEAR);  // GU_LINEAR good for scaling

    // Keep this until we get rev 2489 (or better) of the SDK
    // See http://code.google.com/p/wagic/issues/detail?id=92
    sceGuSendCommandi(210, BUFFER_FORMAT);
}

void JRenderer::EndScene() {
    sceGuFinish();

    sceGuSync(0, 0);

    if (mVsync) sceDisplayWaitVblankStart();

    sceGuSwapBuffers();

    mCurrentTex = -1;
    mCurrentBlend = -1;
}

void JRenderer::EnableVSync(bool flag) { mVsync = flag; }

void JRenderer::ClearScreen(PIXEL_TYPE color) {
    static PIXEL_TYPE previousColor = 0xFFFFFFFF;
    if (previousColor != color) {
        sceGuClearColor(color);
        sceGuClear(GU_COLOR_BUFFER_BIT);
        previousColor = color;
    }
}

void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(2 * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    vertices[1].color = color;
    vertices[1].x = x + width;
    vertices[1].y = y + height;
    vertices[1].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_SPRITES, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE* colors) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(4 * sizeof(struct VertexColor));

    vertices[0].color = colors[0];
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    vertices[1].color = colors[1];
    vertices[1].x = x + width;
    vertices[1].y = y;
    vertices[1].z = 0.0f;

    vertices[2].color = colors[2];
    vertices[2].x = x;
    vertices[2].y = y + height;
    vertices[2].z = 0.0f;

    vertices[3].color = colors[3];
    vertices[3].x = x + width;
    vertices[3].y = y + height;
    vertices[3].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_TRIANGLE_STRIP, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::DrawRect(float x, float y, float width, float height, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(5 * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    vertices[1].color = color;
    vertices[1].x = x;
    vertices[1].y = y + height;
    vertices[1].z = 0.0f;

    vertices[2].color = color;
    vertices[2].x = x + width;
    vertices[2].y = y + height;
    vertices[2].z = 0.0f;

    vertices[3].color = color;
    vertices[3].x = x + width;
    vertices[3].y = y;
    vertices[3].z = 0.0f;

    vertices[4].color = color;
    vertices[4].x = x;
    vertices[4].y = y;
    vertices[4].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_LINE_STRIP, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 5, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(2 * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x1;
    vertices[0].y = y1;
    vertices[0].z = 0.0f;

    vertices[1].color = color;
    vertices[1].x = x2;
    vertices[1].y = y2;
    vertices[1].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_LINES, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::Plot(float x, float y, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(1 * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_POINTS, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 1, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::PlotArray(float* x, float* y, int count, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(count * sizeof(struct VertexColor));

    for (int i = 0; i < count; i++) {
        vertices[i].color = color;
        vertices[i].x = x[i];
        vertices[i].y = y[i];
        vertices[i].z = 0.0f;
    }

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_POINTS, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, count, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

//////////////////////////////////////////////////////////////////////////
//
//		v1---v2
//			/
//		   /
//		v3---v4
void JRenderer::RenderQuad(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale) {
    if (quad == NULL) {
        WGE_LOG_WARN("NULL quad pointer!");
        return;
    }
    if (quad->mTex == NULL) {
        WGE_LOG_WARN("invalid texture!");
        return;
    }

    if (mCurrentTextureFormat != quad->mTex->mTextureFormat) {
        mCurrentTextureFormat = quad->mTex->mTextureFormat;
        sceGuTexMode(mCurrentTextureFormat, 0, 0, mSwizzle);
    }

    if (mCurrentTex != quad->mTex->mTexId) {
        sceGuTexImage(0, quad->mTex->mTexWidth, quad->mTex->mTexHeight, quad->mTex->mTexWidth, quad->mTex->mBits);
        mCurrentTex = quad->mTex->mTexId;
    }

    if (mCurrentBlend != quad->mBlend) {
        sceGuTexFunc(quad->mBlend, GU_TCC_RGBA);
        mCurrentBlend = quad->mBlend;
    }

    // float destWidth = quad->mWidth*quad->mScaleX;
    float destHeight = quad->mHeight * yScale;
    float x = xo - quad->mHotSpotX * xScale;
    float y = yo - quad->mHotSpotY * yScale;

    float start, end;

    float width;
    float destWidth;
    float fixedWidth = SLICE_SIZE_F * xScale;
    float xx, yy;
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    if (quad->mHFlipped)  // || quad->mVFlipped)
    {
        for (end = quad->mX, start = quad->mX + quad->mWidth; start > end; start -= SLICE_SIZE_F) {
            // allocate memory on the current display list for temporary storage
            // in order to rotate, we use 4 vertices this time
            struct Vertex* vertices = (struct Vertex*)sceGuGetMemory(4 * sizeof(struct Vertex));
            if ((start - SLICE_SIZE_F) > end) {
                width = SLICE_SIZE_F;
                destWidth = fixedWidth;
            } else {
                width = start - end;
                destWidth = width * xScale;
            }

            vertices[0].u = start;
            vertices[0].v = quad->mY;
            vertices[0].color = quad->mColor[0];  //.color;
            vertices[0].x = x;
            vertices[0].y = y;
            vertices[0].z = 0.0f;

            vertices[2].u = start - width;
            vertices[2].v = quad->mY;
            vertices[2].color = quad->mColor[2];  //.color;
            vertices[2].x = x + destWidth;
            vertices[2].y = y;
            vertices[2].z = 0.0f;

            vertices[1].u = start;
            vertices[1].v = quad->mY + quad->mHeight;
            vertices[1].color = quad->mColor[1];  //.color;
            vertices[1].x = x;
            vertices[1].y = y + destHeight;
            vertices[1].z = 0.0f;

            vertices[3].u = start - width;
            vertices[3].v = quad->mY + quad->mHeight;
            vertices[3].color = quad->mColor[3];  //.color;
            vertices[3].x = x + destWidth;
            vertices[3].y = y + destHeight;
            vertices[3].z = 0.0f;

            if (quad->mVFlipped) {
                std::swap(vertices[0].v, vertices[2].v);
                std::swap(vertices[1].v, vertices[3].v);
            }

            if (angle != 0.0f) {
                for (int i = 0; i < 4; i++) {
                    xx = (cosAngle * (vertices[i].x - xo) - sinAngle * (vertices[i].y - yo) + xo);
                    yy = (sinAngle * (vertices[i].x - xo) + cosAngle * (vertices[i].y - yo) + yo);
                    vertices[i].x = xx;
                    vertices[i].y = yy;
                }
            }

            x += destWidth;

            sceGuDrawArray(GU_TRIANGLE_STRIP,
                           GU_TEXTURE_32BITF | TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0,
                           vertices);
        }
    } else {
        for (start = quad->mX, end = quad->mX + quad->mWidth; start < end; start += SLICE_SIZE_F) {
            // allocate memory on the current display list for temporary storage
            // in order to rotate, we use 4 vertices this time
            struct Vertex* vertices = (struct Vertex*)sceGuGetMemory(4 * sizeof(struct Vertex));
            if ((start + SLICE_SIZE_F) < end) {
                width = SLICE_SIZE_F;
                destWidth = fixedWidth;
            } else {
                width = end - start;
                destWidth = width * xScale;
            }

            vertices[0].u = start;
            vertices[0].v = quad->mY;
            vertices[0].color = quad->mColor[0];  //.color;
            vertices[0].x = x;
            vertices[0].y = y;
            vertices[0].z = 0.0f;

            vertices[2].u = start + width;
            vertices[2].v = quad->mY;
            vertices[2].color = quad->mColor[2];  //.color;
            vertices[2].x = x + destWidth;
            vertices[2].y = y;
            vertices[2].z = 0.0f;

            vertices[1].u = start;
            vertices[1].v = quad->mY + quad->mHeight;
            vertices[1].color = quad->mColor[1];  //.color;
            vertices[1].x = x;
            vertices[1].y = y + destHeight;
            vertices[1].z = 0.0f;

            vertices[3].u = start + width;
            vertices[3].v = quad->mY + quad->mHeight;
            vertices[3].color = quad->mColor[3];  //.color;
            vertices[3].x = x + destWidth;
            vertices[3].y = y + destHeight;
            vertices[3].z = 0.0f;

            if (quad->mVFlipped) {
                std::swap(vertices[0].v, vertices[2].v);
                std::swap(vertices[1].v, vertices[3].v);
            }

            if (angle != 0.0f) {
                for (int i = 0; i < 4; i++) {
                    xx = (cosAngle * (vertices[i].x - xo) - sinAngle * (vertices[i].y - yo) + xo);
                    yy = (sinAngle * (vertices[i].x - xo) + cosAngle * (vertices[i].y - yo) + yo);
                    vertices[i].x = xx;
                    vertices[i].y = yy;
                }
            }

            x += destWidth;

            sceGuDrawArray(GU_TRIANGLE_STRIP,
                           GU_TEXTURE_32BITF | TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0,
                           vertices);
        }
    }
}

void JRenderer::RenderQuad(JQuad* quad, VertexColor* points) {
    if (mCurrentTextureFormat != quad->mTex->mTextureFormat) {
        mCurrentTextureFormat = quad->mTex->mTextureFormat;
        sceGuTexMode(mCurrentTextureFormat, 0, 0, mSwizzle);
    }

    if (mCurrentTex != quad->mTex->mTexId) {
        sceGuTexImage(0, quad->mTex->mTexWidth, quad->mTex->mTexHeight, quad->mTex->mTexWidth, quad->mTex->mBits);
        mCurrentTex = quad->mTex->mTexId;
    }

    if (mCurrentBlend != quad->mBlend) {
        sceGuTexFunc(quad->mBlend, GU_TCC_RGBA);
        mCurrentBlend = quad->mBlend;
    }

    // allocate memory on the current display list for temporary storage
    // in order to rotate, we use 4 vertices this time
    struct Vertex* vertices = (struct Vertex*)sceGuGetMemory(4 * sizeof(struct Vertex));

    vertices[0].u = quad->mX;
    vertices[0].v = quad->mY;

    vertices[1].u = quad->mX;
    vertices[1].v = quad->mY + quad->mHeight;

    vertices[2].u = quad->mX + quad->mWidth;
    vertices[2].v = quad->mY;

    vertices[3].u = quad->mX + quad->mWidth;
    vertices[3].v = quad->mY + quad->mHeight;

    vertices[0].color = points[3].color;
    vertices[0].x = points[3].x;
    vertices[0].y = points[3].y;
    vertices[0].z = points[3].z;

    vertices[1].color = points[0].color;
    vertices[1].x = points[0].x;
    vertices[1].y = points[0].y;
    vertices[1].z = points[0].z;

    vertices[2].color = points[2].color;
    vertices[2].x = points[2].x;
    vertices[2].y = points[2].y;
    vertices[2].z = points[2].z;

    vertices[3].color = points[1].color;
    vertices[3].x = points[1].x;
    vertices[3].y = points[1].y;
    vertices[3].z = points[1].z;

    sceGuDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF | TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4,
                   0, vertices);
}

/*
** No-op on PSP.  This is purely a PC openGL utility.
*/
void JRenderer::TransferTextureToGLContext(JTexture& inTexture) {}

//------------------------------------------------------------------------------------------------
// Taken from:
// http://svn.ps2dev.org/filedetails.php?repname=psp&path=/trunk/libpng/screenshot/main.c&rev=0&sc=0
// Save current visible screen as PNG
//------------------------------------------------------------------------------------------------
void JRenderer::ScreenShot(const char* filename) {
    u32* vram32;
    u16* vram16;
    void* temp;
    int bufferwidth;
    int pixelformat;
    int i, x, y;
    png_structp png_ptr;
    png_infop info_ptr;
    FILE* fp;
    u8* line;

    fp = fopen(filename, "wb");
    if (!fp) return;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return;
    }
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, SCREEN_WIDTH, SCREEN_HEIGHT, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);
    line = (u8*)malloc(SCREEN_WIDTH * 3);
    sceDisplayWaitVblankStart();  // if framebuf was set with PSP_DISPLAY_SETBUF_NEXTFRAME, wait until it is changed
    sceDisplayGetFrameBuf(&temp, &bufferwidth, &pixelformat, PSP_DISPLAY_SETBUF_NEXTFRAME);
    // temp = (void*)(0x04000000+0x40000000);
    vram32 = (u32*)temp;
    vram16 = (u16*)vram32;
    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (i = 0, x = 0; x < SCREEN_WIDTH; x++) {
            u32 color = 0;
            u8 r = 0, g = 0, b = 0;
            switch (pixelformat) {
            case PSP_DISPLAY_PIXEL_FORMAT_565:
                color = vram16[x + y * bufferwidth];
                r = (color & 0x1f) << 3;
                g = ((color >> 5) & 0x3f) << 2;
                b = ((color >> 11) & 0x1f) << 3;
                break;
            case PSP_DISPLAY_PIXEL_FORMAT_5551:
                color = vram16[x + y * bufferwidth];
                r = (color & 0x1f) << 3;
                g = ((color >> 5) & 0x1f) << 3;
                b = ((color >> 10) & 0x1f) << 3;
                break;
            case PSP_DISPLAY_PIXEL_FORMAT_4444:
                color = vram16[x + y * bufferwidth];
                r = (color & 0xf) << 4;
                g = ((color >> 4) & 0xf) << 4;
                b = ((color >> 8) & 0xf) << 4;
                break;
            case PSP_DISPLAY_PIXEL_FORMAT_8888:
                color = vram32[x + y * bufferwidth];
                r = color & 0xff;
                g = (color >> 8) & 0xff;
                b = (color >> 16) & 0xff;
                break;
            }
            line[i++] = r;
            line[i++] = g;
            line[i++] = b;
        }
        png_write_row(png_ptr, line);
    }
    free(line);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    fclose(fp);
}

inline constexpr wge::video::pixel_format to_pixel_format(PspDisplayPixelFormats psp_format) noexcept {
    switch (psp_format) {
    case PSP_DISPLAY_PIXEL_FORMAT_565:
        return wge::video::pixel_format::bgr_5650;
    case PSP_DISPLAY_PIXEL_FORMAT_5551:
        return wge::video::pixel_format::abgr_5551;
    case PSP_DISPLAY_PIXEL_FORMAT_4444:
        return wge::video::pixel_format::abgr_4444;
    case PSP_DISPLAY_PIXEL_FORMAT_8888:
        return wge::video::pixel_format::abgr_8888;
    }
    return wge::video::pixel_format::none;
}

int JRenderer::PixelSize(int textureMode) {
    switch (textureMode) {
    case GU_PSM_5650:
        return wge::video::pixel_converter<wge::video::pixel_format::bgr_5650>::pixel_size;
    case GU_PSM_5551:
        return wge::video::pixel_converter<wge::video::pixel_format::abgr_5551>::pixel_size;
    case GU_PSM_4444:
        return wge::video::pixel_converter<wge::video::pixel_format::abgr_4444>::pixel_size;
    case GU_PSM_8888:
        return wge::video::pixel_converter<wge::video::pixel_format::abgr_8888>::pixel_size;
    }
    return PIXEL_SIZE;
}

JTexture* JRenderer::LoadTexture(const char* filename, int mode, int textureMode) {
    auto pixel_format = to_pixel_format(static_cast<PspDisplayPixelFormats>(textureMode));
    if (strstr(filename, ".png") != NULL || strstr(filename, ".PNG") != NULL) {
        textureMode = TEXTURE_FORMAT;  // textureMode not supported in PNG yet
        pixel_format = to_pixel_format(static_cast<PspDisplayPixelFormats>(PIXEL_FORMAT));
    }

    auto* fileSystem = JFileSystem::GetInstance();
    if (!fileSystem->OpenFile(filename)) {
        WGE_LOG_ERROR("could not open {}", filename);
        return nullptr;
    }

    const bool use_vram = (mode == TEX_TYPE_USE_VRAM);
    auto& filestream = fileSystem->current_file();

    auto data = wge::video::image_loader::load_image(filestream, pixel_format, use_vram, mSwizzle);
    fileSystem->CloseFile();

    if (!data.pixels) {
        WGE_LOG_ERROR("could not load {}", filename);
        return nullptr;
    }

    if (mImageFilter != nullptr) {
        mImageFilter->ProcessImage(reinterpret_cast<PIXEL_TYPE*>(data.pixels.get()), data.width, data.height);
    }

    auto* tex = new JTexture();
    if (tex) {
        tex->mTexId = mTexCounter++;
        tex->mTextureFormat = textureMode;

        tex->mWidth = data.width;
        tex->mHeight = data.height;
        tex->mTexWidth = data.texture_width;
        tex->mTexHeight = data.texture_height;

        tex->mInVideoRAM = data.pixels.is_vram();
        tex->mBits = reinterpret_cast<PIXEL_TYPE*>(data.pixels.release());
    }

    WGE_LOG_TRACE("-- OK  --");
    return tex;
}

JTexture* JRenderer::CreateTexture(int width, int height, int mode) {
    bool useVideoRAM = (mode == TEX_TYPE_USE_VRAM);

    JTexture* tex = new JTexture();
    if (tex) {
        tex->mWidth = width;
        tex->mHeight = height;

        tex->mTexWidth = wge::math::nearest_superior_power_of_2(width);
        tex->mTexHeight = wge::math::nearest_superior_power_of_2(height);

        int size = tex->mTexWidth * tex->mTexHeight * sizeof(PIXEL_TYPE);
        if (useVideoRAM) {
            tex->mInVideoRAM = true;
            tex->mBits = (PIXEL_TYPE*)valloc(size);
        }

        // else
        if (tex->mBits == NULL) {
            tex->mInVideoRAM = false;
            tex->mBits = (PIXEL_TYPE*)memalign(16, size);
        }

        memset(tex->mBits, 0, size);

        tex->mTexId = mTexCounter++;
    }

    return tex;
}

void JRenderer::BindTexture(JTexture* tex) {
    if (mCurrentTex != tex->mTexId) {
        sceGuTexImage(0, tex->mTexWidth, tex->mTexHeight, tex->mTexWidth, tex->mBits);
        mCurrentTex = tex->mTexId;

        if (m3DEnabled) {
            if (mCurrentRenderMode == MODE_3D) {
                sceKernelDcacheWritebackAll();
                sceGuTexFunc(GU_TFX_ADD, GU_TCC_RGB);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void JRenderer::Enable2D() {
    mCurrentRenderMode = MODE_2D;

    sceGuDisable(GU_DEPTH_TEST);
}

//////////////////////////////////////////////////////////////////////////
void JRenderer::Enable3D() {
    mCurrentRenderMode = MODE_3D;

    mCurrentBlend = -1;

    sceGuEnable(GU_DEPTH_TEST);

    LoadIdentity();
}

//////////////////////////////////////////////////////////////////////////
void JRenderer::SetClip(int x, int y, int width, int height) { sceGuScissor(x, y, width, height); }

void JRenderer::LoadIdentity() {
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
}

void JRenderer::Translate(float x, float y, float z) {
    ScePspFVector3 pos = {x, y, z};
    sceGumTranslate(&pos);
}

void JRenderer::RotateX(float angle) { sceGumRotateX(angle); }

void JRenderer::RotateY(float angle) { sceGumRotateY(angle); }

void JRenderer::RotateZ(float angle) { sceGumRotateZ(angle); }

void JRenderer::PushMatrix() { sceGumPushMatrix(); }

void JRenderer::PopMatrix() { sceGumPopMatrix(); }

void JRenderer::RenderTriangles(JTexture* texture, Vertex3D* tris, int start, int count) {
    if (texture) BindTexture(texture);

    PSPVertex3D* vertices = (PSPVertex3D*)sceGuGetMemory(count * 3 * sizeof(PSPVertex3D));

    int n = 0;
    int index = start * 3;
    for (int i = 0; i < count; i++) {
        vertices[n].texture.x = tris[index].u;
        vertices[n].texture.y = tris[index].v;

        vertices[n].pos.x = tris[index].x;
        vertices[n].pos.y = tris[index].y;
        vertices[n].pos.z = tris[index].z;

        index++;
        n++;

        vertices[n].texture.x = tris[index].u;
        vertices[n].texture.y = tris[index].v;

        vertices[n].pos.x = tris[index].x;
        vertices[n].pos.y = tris[index].y;
        vertices[n].pos.z = tris[index].z;

        index++;
        n++;

        vertices[n].texture.x = tris[index].u;
        vertices[n].texture.y = tris[index].v;

        vertices[n].pos.x = tris[index].x;
        vertices[n].pos.y = tris[index].y;
        vertices[n].pos.z = tris[index].z;

        index++;
        n++;
    }

    sceGuColor(0xff000000);
    sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, count * 3, 0, vertices);
}

void JRenderer::SetFOV(float fov) {
    mFOV = fov;

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(mFOV, 16.0f / 9.0f, 0.5f, 1000.0f);
}

void JRenderer::FillPolygon(float* x, float* y, int count, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(count * sizeof(struct VertexColor));

    for (int i = 0; i < count; i++) {
        vertices[i].color = color;
        vertices[i].x = x[i];
        vertices[i].y = y[i];
        vertices[i].z = 0.0f;
    }

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_TRIANGLE_FAN, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, count, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::DrawPolygon(float* x, float* y, int count, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory((count + 1) * sizeof(struct VertexColor));

    for (int i = 0; i < count; i++) {
        vertices[i].color = color;
        vertices[i].x = x[i];
        vertices[i].y = y[i];
        vertices[i].z = 0.0f;
    }

    vertices[count].color = color;
    vertices[count].x = x[0];
    vertices[count].y = y[0];
    vertices[count].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_LINE_STRIP, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, count + 1, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, float lineWidth, PIXEL_TYPE color) {
    float dy = y2 - y1;
    float dx = x2 - x1;
    if (dy == 0 && dx == 0) return;

    float l = (float)hypot(dx, dy);

    float x[4];
    float y[4];

    x[0] = x1 + lineWidth * (y2 - y1) / l;
    y[0] = y1 - lineWidth * (x2 - x1) / l;

    x[1] = x1 - lineWidth * (y2 - y1) / l;
    y[1] = y1 + lineWidth * (x2 - x1) / l;

    x[2] = x2 - lineWidth * (y2 - y1) / l;
    y[2] = y2 + lineWidth * (x2 - x1) / l;

    x[3] = x2 + lineWidth * (y2 - y1) / l;
    y[3] = y2 - lineWidth * (x2 - x1) / l;

    FillPolygon(x, y, 4, color);
}

void JRenderer::DrawCircle(float x, float y, float radius, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(181 * sizeof(struct VertexColor));

    int angle = 359;
    for (int i = 0; i < 180; i++) {
        vertices[i].color = color;
        vertices[i].x = x + radius * COSF(angle);
        vertices[i].y = y + radius * SINF(angle);
        vertices[i].z = 0.0f;
        angle -= 2;
        if (angle < 0) angle = 0;
    }

    vertices[180].color = color;
    vertices[180].x = x + radius * COSF(0);
    vertices[180].y = y + radius * SINF(0);
    vertices[180].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_LINE_STRIP, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 181, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::FillCircle(float x, float y, float radius, PIXEL_TYPE color) {
    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(182 * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    int angle = 359;
    for (int i = 0; i < 180; i++) {
        vertices[i + 1].color = color;
        vertices[i + 1].x = x + radius * COSF(angle);
        vertices[i + 1].y = y + radius * SINF(angle);
        vertices[i + 1].z = 0.0f;
        angle -= 2;
        if (angle < 0) angle = 0;
    }

    vertices[181].color = color;
    vertices[181].x = x + radius * COSF(359);
    vertices[181].y = y + radius * SINF(359);
    vertices[181].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_TRIANGLE_FAN, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 182, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::DrawPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color) {
    float angle = startingAngle * RAD2DEG;
    float firstAngle = angle;
    float steps = 360.0f / count;
    size /= 2;

    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory((count + 1) * sizeof(struct VertexColor));

    for (int i = 0; i < count; i++) {
        vertices[i].color = color;
        vertices[i].x = x + size * COSF((int)angle);
        vertices[i].y = y + size * SINF((int)angle);
        vertices[i].z = 0.0f;
        angle -= steps;
        if (angle < 0.0f) angle += 360.0f;
    }

    vertices[count].color = color;
    vertices[count].x = x + size * COSF((int)firstAngle);
    vertices[count].y = y + size * SINF((int)firstAngle);
    vertices[count].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_LINE_STRIP, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, count + 1, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::FillPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color) {
    float angle = startingAngle * RAD2DEG;
    float firstAngle = angle;
    float steps = 360.0f / count;
    size /= 2;

    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory((count + 2) * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    for (int i = 0; i < count; i++) {
        vertices[i + 1].color = color;
        vertices[i + 1].x = x + size * COSF((int)angle);
        vertices[i + 1].y = y + size * SINF((int)angle);
        vertices[i + 1].z = 0.0f;
        angle -= steps;
        if (angle < 0.0f) angle += 360.0f;
    }

    vertices[count + 1].color = color;
    vertices[count + 1].x = x + size * COSF((int)firstAngle);
    vertices[count + 1].y = y + size * SINF((int)firstAngle);
    vertices[count + 1].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_TRIANGLE_FAN, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, count + 2, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::DrawRoundRect(float x1, float y1, float w, float h, float radius, PIXEL_TYPE color) {
    float x2 = x1 + w;
    float y2 = y1 + h;
    for (int i = -radius; i < y2 - y1 + radius; i++) {
        float q = radius;
        float nextq = q + 1;
        if (i < 0) {
            q = (float)sqrt(radius * radius - (-i) * (-i));
            nextq = (float)sqrt(radius * radius - (-i + 1) * (-i + 1));
        } else if (i > y2 - y1) {
            q = (float)sqrt(radius * radius - (i - (y2 - y1)) * (i - (y2 - y1)));
            nextq = (float)sqrt(radius * radius - (i + 1 - (y2 - y1)) * (i + 1 - (y2 - y1)));
        }
        if (nextq == q) nextq = q + 1;
        if (i == -radius || i == y2 - y1 + radius - 1) {
            DrawLine(x1 + (radius - q), y1 + i + radius, x2 + q + radius, y1 + i + radius, color);
        } else {
            DrawLine(x1 + (radius - q), y1 + i + radius, x1 + (radius - nextq), y1 + i + radius, color);
            DrawLine(x2 + radius + q, y1 + i + radius, x2 + radius + nextq, y1 + i + radius, color);
        }
    }
}

void JRenderer::FillRoundRect(float x, float y, float w, float h, float radius, PIXEL_TYPE color) {
    x += w + radius;
    y += radius;

    struct VertexColor* vertices = (struct VertexColor*)sceGuGetMemory(182 * sizeof(struct VertexColor));

    vertices[0].color = color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    int angle = 359;
    for (int i = 0; i < 45; i++) {
        vertices[i + 1].color = color;
        vertices[i + 1].x = x + radius * COSF(angle);
        vertices[i + 1].y = y + radius * SINF(angle);
        vertices[i + 1].z = 0.0f;
        angle -= 2;
        if (angle < 0) angle = 0;
    }

    x -= w;

    for (int i = 45; i < 90; i++) {
        vertices[i + 1].color = color;
        vertices[i + 1].x = x + radius * COSF(angle);
        vertices[i + 1].y = y + radius * SINF(angle);
        vertices[i + 1].z = 0.0f;
        angle -= 2;
        if (angle < 0) angle = 0;
    }

    y += h;
    for (int i = 90; i < 135; i++) {
        vertices[i + 1].color = color;
        vertices[i + 1].x = x + radius * COSF(angle);
        vertices[i + 1].y = y + radius * SINF(angle);
        vertices[i + 1].z = 0.0f;
        angle -= 2;
        if (angle < 0) angle = 0;
    }

    x += w;
    for (int i = 135; i < 180; i++) {
        vertices[i + 1].color = color;
        vertices[i + 1].x = x + radius * COSF(angle);
        vertices[i + 1].y = y + radius * SINF(angle);
        vertices[i + 1].z = 0.0f;
        angle -= 2;
        if (angle < 0) angle = 0;
    }

    y -= h;
    vertices[181].color = color;
    vertices[181].x = x + radius * COSF(359);
    vertices[181].y = y + radius * SINF(359);
    vertices[181].z = 0.0f;

    sceGuDisable(GU_TEXTURE_2D);
    sceGuShadeModel(GU_SMOOTH);
    sceGuAmbientColor(0xffffffff);
    sceGuDrawArray(GU_TRIANGLE_FAN, TEXTURE_COLOR_FORMAT | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 182, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}

void JRenderer::SetImageFilter(JImageFilter* imageFilter) { mImageFilter = imageFilter; }
