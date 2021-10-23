//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
#include "JGE.h"
#include "JRenderer.h"
#include "JAssert.h"

JQuad::JQuad(JTexture* tex, float x, float y, float width, float height)
    : mTex(tex), mX(x), mY(y), mWidth(width), mHeight(height) {}

void JQuad::SetTextureRect(float x, float y, float w, float h) {
    mX      = x;
    mY      = y;
    mWidth  = w;
    mHeight = h;
}

void JQuad::GetTextureRect(float* x, float* y, float* w, float* h) const {
    *x = mX;
    *y = mY;
    *w = mWidth;
    *h = mHeight;
}

void JQuad::SetColor(PIXEL_TYPE color) {
    for (int i = 0; i < 4; i++) mColor[i].color = color;
}

void JQuad::SetHotSpot(float x, float y) {
    mHotSpotX = x;
    mHotSpotY = y;
}

//////////////////////////////////////////////////////////////////////////

JTexture::JTexture() : mWidth(0), mHeight(0) {}

JTexture::~JTexture() {}

void JTexture::UpdateBits(int x, int y, int width, int height, PIXEL_TYPE* bits) {
    JRenderer::GetInstance()->BindTexture(this);
}

//////////////////////////////////////////////////////////////////////////

JRenderer* JRenderer::mInstance = NULL;
bool JRenderer::m3DEnabled      = false;

void JRenderer::Set3DFlag(bool flag) { m3DEnabled = flag; }

JRenderer* JRenderer::GetInstance() {
    if (mInstance == NULL) {
        mInstance = new JRenderer();
        JASSERT(mInstance != NULL);
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

JRenderer::JRenderer() : mActualWidth(SCREEN_WIDTH_F), mActualHeight(SCREEN_HEIGHT_F) {}

JRenderer::~JRenderer() {}

void JRenderer::InitRenderer() {
    mCurrentTextureFilter = TEX_FILTER_NONE;
    mImageFilter          = NULL;

    mCurrTexBlendSrc  = BLEND_SRC_ALPHA;
    mCurrTexBlendDest = BLEND_ONE_MINUS_SRC_ALPHA;

    //	mLineWidth = 1.0f;
    mFOV        = 75.0f;

    mCurrentRenderMode = MODE_UNKNOWN;
}

void JRenderer::DestroyRenderer() const {}

void JRenderer::BeginScene() {}

void JRenderer::EndScene() {}

void JRenderer::BindTexture(JTexture* tex) {}

void JRenderer::EnableTextureFilter(bool flag) {}

void Swap(float* a, float* b) {
    float n = *a;
    *a      = *b;
    *b      = n;
}

void JRenderer::RenderQuad(JQuad* quad, float xo, float yo, float angle, float xScale, float yScale) {}

void JRenderer::RenderQuad(JQuad* quad, VertexColor* pt) {
    for (int i = 0; i < 4; i++) {
        pt[i].y               = SCREEN_HEIGHT_F - pt[i].y;
        quad->mColor[i].color = pt[i].color;
    }
    BindTexture(quad->mTex);
}

void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE color) {}

void JRenderer::DrawRect(float x, float y, float width, float height, PIXEL_TYPE color) {}

void JRenderer::FillRect(float x, float y, float width, float height, PIXEL_TYPE* colors) {}

void JRenderer::FillRect(float x, float y, float width, float height, JColor* colors) {}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, PIXEL_TYPE color) {}

void JRenderer::Plot(float x, float y, PIXEL_TYPE color) {}

void JRenderer::PlotArray(float* x, float* y, int count, PIXEL_TYPE color) {}

void JRenderer::ScreenShot(const char* filename __attribute__((unused))) {}

void JRenderer::TransferTextureToGLContext(JTexture& inTexture) {}

JTexture* JRenderer::CreateTexture(int width, int height, int mode __attribute__((unused))) { return new JTexture(); }

JTexture* JRenderer::LoadTexture(const char* filename, int mode, int textureFormat) { return new JTexture(); }

void JRenderer::EnableVSync(bool flag __attribute__((unused))) {}

void JRenderer::ClearScreen(PIXEL_TYPE color) {}

void JRenderer::SetTexBlend(int src, int dest) {}

void JRenderer::SetTexBlendSrc(int src) {}

void JRenderer::SetTexBlendDest(int dest) {}

void JRenderer::Enable2D() {}

void JRenderer::Enable3D() {}

void JRenderer::SetClip(int, int, int, int) {}

void JRenderer::LoadIdentity() {}

void JRenderer::Translate(float, float, float) {}

void JRenderer::RotateX(float) {}

void JRenderer::RotateY(float) {}

void JRenderer::RotateZ(float) {}

void JRenderer::PushMatrix() {}

void JRenderer::PopMatrix() {}

void JRenderer::RenderTriangles(JTexture* texture, Vertex3D* vertices, int start, int count) {}

void JRenderer::SetFOV(float fov) { mFOV = fov; }

void JRenderer::FillPolygon(float* x, float* y, int count, PIXEL_TYPE color) {}

void JRenderer::DrawPolygon(float* x, float* y, int count, PIXEL_TYPE color) {}

void JRenderer::DrawLine(float x1, float y1, float x2, float y2, float lineWidth, PIXEL_TYPE color) {}

void JRenderer::DrawCircle(float x, float y, float radius, PIXEL_TYPE color) {}

void JRenderer::FillCircle(float x, float y, float radius, PIXEL_TYPE color) {}

void JRenderer::DrawPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color) {}

void JRenderer::FillPolygon(float x, float y, float size, int count, float startingAngle, PIXEL_TYPE color) {}

void JRenderer::SetImageFilter(JImageFilter* imageFilter) { mImageFilter = imageFilter; }

void JRenderer::DrawRoundRect(float x, float y, float w, float h, float radius, PIXEL_TYPE color) {}

void JRenderer::FillRoundRect(float x, float y, float w, float h, float radius, PIXEL_TYPE color) {}
