#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#ifdef WIN32
#undef GL_VERSION_2_0
#endif

#include "JGE.h"
#include "JTypes.h"
#include "JApp.h"
#include "JFileSystem.h"
#include "JRenderer.h"
#include "JGameLauncher.h"

#include <wge/log.hpp>

#include <stdexcept>
#include <math.h>

#if (defined FORCE_GLES)
#undef GL_ES_VERSION_2_0
#undef GL_VERSION_2_0
#define GL_VERSION_ES_CM_1_1 1
#define glOrthof glOrtho
#define glClearDepthf glClearDepth
#endif

#define ACTUAL_SCREEN_WIDTH (SCREEN_WIDTH)
#define ACTUAL_SCREEN_HEIGHT (SCREEN_HEIGHT)
#define ACTUAL_RATIO ((GLfloat)ACTUAL_SCREEN_WIDTH / (GLfloat)ACTUAL_SCREEN_HEIGHT)

enum eDisplayMode { DisplayMode_lowRes = 0, DisplayMode_hiRes, DisplayMode_fullscreen };

unsigned int gDisplayMode = DisplayMode_lowRes;

const int kHitzonePliancy = 50;

// tick value equates to ms
const int kTapEventTimeout = 250;

uint64_t lastTickCount;
JGE* g_engine = NULL;
JApp* g_app = NULL;
JGameLauncher* g_launcher = NULL;

class SdlApp;

SdlApp* g_SdlApp = NULL;

class SdlApp {
public: /* For easy interfacing with JGE static functions */
    bool Running;
    SDL_Window* window;
    SDL_GLContext gl_context;
    SDL_Rect viewPort;
    Uint32 lastMouseUpTime;
    Uint32 lastFingerDownTime;
    int windowed_w;
    int windowed_h;
    int windowed_pos_x;
    int windowed_pos_y;

    int mMouseDownX;
    int mMouseDownY;
    static SdlApp* sInstance;

public:
    SdlApp()
        : Running(true),
          window(NULL),
          gl_context(NULL),
          lastMouseUpTime(0),
          lastFingerDownTime(0),
          mMouseDownX(0),
          mMouseDownY(0) {
        sInstance = this;
    }

    static void OneIter() {
        SDL_Event Event;
        if (g_engine) {
            for (int x = 0; x < 5 && SDL_WaitEventTimeout(&Event, 10); ++x) {
                if (!g_engine->IsPaused()) sInstance->OnEvent(&Event);
            }
            if (!g_engine->IsPaused()) sInstance->OnUpdate();
        }
    }

    int OnExecute() {
        if (OnInit() == false) {
            return -1;
        }

        while (Running) {
            OneIter();
        }
        OnCleanup();

        return 0;
    }

public:
    bool OnInit();

    void OnResize(int width, int height) {
        WGE_LOG_DEBUG("width {} height {}", width, height);

        if ((GLfloat)width / (GLfloat)height <= ACTUAL_RATIO) {
            viewPort.x = 0;
            viewPort.y = -(static_cast<int>(width / ACTUAL_RATIO) - height) / 2;
            viewPort.w = width;
            viewPort.h = static_cast<int>(width / ACTUAL_RATIO);
        } else {
            viewPort.x = -(static_cast<int>(height * ACTUAL_RATIO) - width) / 2;
            viewPort.y = 0;
            viewPort.w = static_cast<int>(height * ACTUAL_RATIO);
            viewPort.h = height;
        }

        glViewport(viewPort.x, viewPort.y, viewPort.w, viewPort.h);

        JRenderer::GetInstance()->SetActualWidth(static_cast<float>(viewPort.w));
        JRenderer::GetInstance()->SetActualHeight(static_cast<float>(viewPort.h));
        glScissor(0, 0, width, height);

#if (!defined GL_ES_VERSION_2_0) && (!defined GL_VERSION_2_0)

        glMatrixMode(GL_PROJECTION);  // Select The Projection Matrix
        glLoadIdentity();             // Reset The Projection Matrix

#if (defined GL_VERSION_ES_CM_1_1)
        glOrthof(0.0f, (float)(viewPort.w) - 1.0f, 0.0f, (float)(viewPort.h) - 1.0f, -1.0f, 1.0f);
#else
        gluOrtho2D(0.0f, (float)(viewPort.w) - 1.0f, 0.0f, (float)(viewPort.h) - 1.0f);
#endif
        glMatrixMode(GL_MODELVIEW);  // Select The Modelview Matrix
        glLoadIdentity();            // Reset The Modelview Matrix

        glDisable(GL_DEPTH_TEST);
#endif
    }

    void OnKeyPressed(const SDL_KeyboardEvent& event);
    void OnMouseDoubleClicked(const SDL_MouseButtonEvent& event);
    void OnMouseClicked(const SDL_MouseButtonEvent& event);
    void OnMouseMoved(const SDL_MouseMotionEvent& event);
    void OnMouseWheel(int x, int y);

    void OnTouchEvent(const SDL_TouchFingerEvent& event);

    void OnEvent(SDL_Event* Event) {
        /* if(Event->type < SDL_USEREVENT)
        DebugTrace("Event received" << Event->type);*/
        switch (Event->type) {
        case SDL_QUIT:
            Running = false;
            break;

        /* On Android, this is triggered when the device orientation changed */
        case SDL_WINDOWEVENT:
            window = SDL_GetWindowFromID(Event->window.windowID);
            int h, w;
            SDL_GetWindowSize(window, &w, &h);
            OnResize(w, h);
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            OnKeyPressed(Event->key);
            break;

        case SDL_MOUSEMOTION:
            OnMouseMoved(Event->motion);
            break;

        case SDL_MOUSEBUTTONDOWN:
            OnMouseClicked(Event->button);
            break;

        case SDL_MOUSEBUTTONUP: {
            Uint32 eventTime = SDL_GetTicks();
            if (eventTime - lastMouseUpTime <= 500) {
                OnMouseDoubleClicked(Event->button);
            } else {
                OnMouseClicked(Event->button);
            }
            lastMouseUpTime = eventTime;
        } break;

        case SDL_MOUSEWHEEL:
            OnMouseWheel(Event->wheel.x, Event->wheel.y);
            break;

        case SDL_FINGERMOTION:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
            WGE_LOG_DEBUG("Finger Up/Down/Motion detected:");
            OnTouchEvent(Event->tfinger);
            break;

        case SDL_MULTIGESTURE:
            WGE_LOG_DEBUG("Multigesture : touchId {}, x {}, y {}, dTheta {}, dDist {}, numFingers {}",
                          Event->mgesture.touchId, Event->mgesture.x, Event->mgesture.y, Event->mgesture.dTheta,
                          Event->mgesture.dDist, Event->mgesture.numFingers);
            break;

        case SDL_JOYBALLMOTION:
            WGE_LOG_DEBUG("Flick gesture detected, x: {}, y: {}", Event->jball.xrel, Event->jball.yrel);
            g_engine->Scroll(Event->jball.xrel, Event->jball.yrel);
            break;
        }
    }

    void OnUpdate();

    void OnCleanup() {
        SDL_GL_DeleteContext(gl_context);
        SDL_Quit();
    }
};
SdlApp* SdlApp::sInstance = 0;

static const struct {
    LocalKeySym keysym;
    JButton keycode;
} gDefaultBindings[] = {
    /* windows controls */
    {SDLK_LCTRL, JGE_BTN_CTRL},
    {SDLK_RCTRL, JGE_BTN_CTRL},
    {SDLK_RETURN, JGE_BTN_MENU},
    {SDLK_KP_ENTER, JGE_BTN_MENU},
    {SDLK_ESCAPE, JGE_BTN_MENU},
    {SDLK_UP, JGE_BTN_UP},
    {SDLK_DOWN, JGE_BTN_DOWN},
    {SDLK_LEFT, JGE_BTN_LEFT},
    {SDLK_RIGHT, JGE_BTN_RIGHT},
    {SDLK_w, JGE_BTN_UP},
    {SDLK_s, JGE_BTN_DOWN},
    {SDLK_a, JGE_BTN_LEFT},
    {SDLK_d, JGE_BTN_RIGHT},
    {SDLK_q, JGE_BTN_PREV},
    {SDLK_e, JGE_BTN_NEXT},
    {SDLK_i, JGE_BTN_CANCEL},
    {SDLK_l, JGE_BTN_OK},
    {SDLK_SPACE, JGE_BTN_OK},
    {SDLK_k, JGE_BTN_SEC},
    {SDLK_j, JGE_BTN_PRI},
    {SDLK_b, JGE_BTN_SOUND},
    {SDLK_f, JGE_BTN_FULLSCREEN},

    /* old Qt ones, basically modified to comply with the N900 keyboard
    { SDLK_a,             JGE_BTN_NEXT },
    { SDLK_TAB,           JGE_BTN_CANCEL },
    { SDLK_q,             JGE_BTN_PREV },
    { SDLK_BACKSPACE,     JGE_BTN_CTRL },
    */

    /* Android customs */
    {SDLK_AC_BACK, JGE_BTN_MENU},
    /* Android/maemo volume button mapping */
    {SDLK_VOLUMEUP, JGE_BTN_PREV},
    {SDLK_VOLUMEDOWN, JGE_BTN_SEC},
};

void JGECreateDefaultBindings() {
    for (signed int i = sizeof(gDefaultBindings) / sizeof(gDefaultBindings[0]) - 1; i >= 0; --i)
        g_engine->BindKey(gDefaultBindings[i].keysym, gDefaultBindings[i].keycode);
}

int JGEGetTime() { return (int)SDL_GetTicks(); }

bool JGEToggleFullscreen() {
    // cycle between the display modes
    ++gDisplayMode;
    if (gDisplayMode > DisplayMode_fullscreen) gDisplayMode = DisplayMode_lowRes;

    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);

    int width = 0;
    int height = 0;

    switch (gDisplayMode) {
    case DisplayMode_fullscreen:
        SDL_SetWindowSize(g_SdlApp->window, mode.w, mode.h);
        return (SDL_SetWindowFullscreen(g_SdlApp->window, SDL_TRUE) == 0);

        break;

    case DisplayMode_hiRes:
        width = SCREEN_WIDTH * 2;
        height = SCREEN_HEIGHT * 2;
        break;

    case DisplayMode_lowRes:
    default:
        width = SCREEN_WIDTH;
        height = SCREEN_HEIGHT;
        break;
    }

    if (SDL_SetWindowFullscreen(g_SdlApp->window, SDL_FALSE) < 0) {
        return false;
    }

    int x = (mode.w - width) / 2;
    int y = (mode.h - height) / 2;

    SDL_SetWindowPosition(g_SdlApp->window, x, y);
    SDL_SetWindowSize(g_SdlApp->window, width, height);

    return true;
}

bool InitGame(void) {
    g_engine = JGE::GetInstance();
    g_app = g_launcher->GetGameApp();
    g_app->Create();
    g_engine->SetApp(g_app);

    JRenderer::GetInstance()->Enable2D();
    lastTickCount = JGEGetTime();

    return true;
}

void DestroyGame(void) {
    if (g_engine != NULL) {
        g_engine->SetApp(NULL);
    }

    if (g_app) {
        g_app->Destroy();
        delete g_app;
        g_app = NULL;
    }

    JGE::Destroy();

    g_engine = NULL;
}

void SdlApp::OnUpdate() {
    static int tickCount = 0;
    tickCount = JGEGetTime();
    int64_t dt = (tickCount - lastTickCount);
    lastTickCount = tickCount;

    if (g_engine->IsDone()) {
        SDL_Event event;
        event.user.type = SDL_QUIT;
        SDL_PushEvent(&event);
    }

    try {
        g_engine->SetDelta((float)dt / 1000.0f);
        g_engine->Update((float)dt / 1000.0f);
    } catch (std::out_of_range& oor) {
        std::cerr << oor.what();
    }

    if (g_engine) g_engine->Render();

    SDL_GL_SwapWindow(window);
}

void SdlApp::OnKeyPressed(const SDL_KeyboardEvent& event) {
    if (event.type == SDL_KEYDOWN) {
        g_engine->HoldKey_NoRepeat((LocalKeySym)event.keysym.sym);
    } else if (event.type == SDL_KEYUP) {
        g_engine->ReleaseKey((LocalKeySym)event.keysym.sym);
    }
}

void SdlApp::OnMouseMoved(const SDL_MouseMotionEvent& event) {
    int actualWidth = (int)JRenderer::GetInstance()->GetActualWidth();
    int actualHeight = (int)JRenderer::GetInstance()->GetActualHeight();

    if (event.y >= viewPort.y && event.y <= viewPort.y + viewPort.h && event.x >= viewPort.x &&
        event.x <= viewPort.x + viewPort.w) {
        g_engine->LeftClicked(((event.x - viewPort.x) * SCREEN_WIDTH) / actualWidth,
                              ((event.y - viewPort.y) * SCREEN_HEIGHT) / actualHeight);
    }
}

void SdlApp::OnMouseDoubleClicked(const SDL_MouseButtonEvent& event) {}

void SdlApp::OnMouseWheel(int x, int y) {
    if (!x && y) {  // Vertical wheel
        if (y > 0) {
            g_engine->HoldKey_NoRepeat(JGE_BTN_UP);
        } else {
            g_engine->HoldKey_NoRepeat(JGE_BTN_DOWN);
        }
    } else if (x && !y) {  // Horizontal wheel
        g_engine->HoldKey_NoRepeat(JGE_BTN_LEFT);
    } else {
        g_engine->HoldKey_NoRepeat(JGE_BTN_RIGHT);
    }
}

void SdlApp::OnMouseClicked(const SDL_MouseButtonEvent& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button == SDL_BUTTON_LEFT) /* Left button */
        {
            // this is intended to convert window coordinate into game coordinate.
            // this is correct only if the game and window have the same aspect ratio, otherwise, it's just wrong
            int actualWidth = (int)JRenderer::GetInstance()->GetActualWidth();
            int actualHeight = (int)JRenderer::GetInstance()->GetActualHeight();

            if (event.y >= viewPort.y && event.y <= viewPort.y + viewPort.h && event.x >= viewPort.x &&
                event.x <= viewPort.x + viewPort.w) {
                g_engine->LeftClicked(((event.x - viewPort.x) * SCREEN_WIDTH) / actualWidth,
                                      ((event.y - viewPort.y) * SCREEN_HEIGHT) / actualHeight);
                g_engine->HoldKey_NoRepeat(JGE_BTN_OK);
            } else if (event.y < viewPort.y) {
                g_engine->HoldKey_NoRepeat(JGE_BTN_MENU);
            } else if (event.y > viewPort.y + viewPort.h) {
                g_engine->HoldKey_NoRepeat(JGE_BTN_NEXT);
            }
        } else if (event.button == SDL_BUTTON_RIGHT) /* Right button */
        {                                            /* next phase please */
            g_engine->HoldKey_NoRepeat(JGE_BTN_PREV);
        } else if (event.button == SDL_BUTTON_MIDDLE) /* Middle button */
        {                                             /* interrupt please */
            g_engine->HoldKey_NoRepeat(JGE_BTN_SEC);
        }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button == SDL_BUTTON_LEFT) {
            if (event.y >= viewPort.y && event.y <= viewPort.y + viewPort.h && event.x >= viewPort.x &&
                event.x <= viewPort.x + viewPort.w) {
                g_engine->ReleaseKey(JGE_BTN_OK);
            } else if (event.y < viewPort.y) {
                g_engine->ReleaseKey(JGE_BTN_MENU);
            } else if (event.y > viewPort.y + viewPort.h) {
                g_engine->ReleaseKey(JGE_BTN_NEXT);
            }
        } else if (event.button == SDL_BUTTON_RIGHT) { /* next phase please */
            g_engine->ReleaseKey(JGE_BTN_PREV);
        } else if (event.button == SDL_BUTTON_MIDDLE) { /* interrupt please */
            g_engine->ReleaseKey(JGE_BTN_SEC);
        }
    }
}

void SdlApp::OnTouchEvent(const SDL_TouchFingerEvent& event) {
    // only respond to the first finger for mouse type movements - any additional finger
    // should be ignored, and will come through instead as a multigesture event
    if (event.fingerId == 0) {
        if (event.y >= viewPort.y && event.y <= viewPort.y + viewPort.h && event.x >= viewPort.x &&
            event.x <= viewPort.x + viewPort.w) {
            int actualWidth = (int)JRenderer::GetInstance()->GetActualWidth();
            int actualHeight = (int)JRenderer::GetInstance()->GetActualHeight();

            Uint32 eventTime = SDL_GetTicks();
            if (event.type == SDL_FINGERDOWN) {
                mMouseDownX = event.x;
                mMouseDownY = event.y;

                lastFingerDownTime = eventTime;
            }

            g_engine->LeftClicked(((event.x - viewPort.x) * SCREEN_WIDTH) / actualWidth,
                                  ((event.y - viewPort.y) * SCREEN_HEIGHT) / actualHeight);
        }
    }
}

bool SdlApp::OnInit() {
    WGE_LOG_TRACE("init started");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    SDL_DisplayMode currentDisplayMode;
    SDL_GetCurrentDisplayMode(0, &currentDisplayMode);
    WGE_LOG_DEBUG("Video Display : h {}, w {}", currentDisplayMode.h, currentDisplayMode.w);

    int window_w = SCREEN_WIDTH;
    int window_h = SCREEN_HEIGHT;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    int buffers, samples;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &buffers);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samples);
    if (buffers == 0 || samples == 0) {  // no multisampling available
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(g_launcher->GetName(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_w,
                              window_h, flags);
    if (window == NULL) {
        WGE_LOG_ERROR( "SDL Error: {}", SDL_GetError());
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Black Background (yes that's the way fuckers)

#if (defined GL_ES_VERSION_2_0) || (defined GL_VERSION_2_0)
#if (defined GL_ES_VERSION_2_0)
    glClearDepthf(1.0f);  // Depth Buffer Setup
#else
    glClearDepth(1.0f);  // Depth Buffer Setup
#endif  // (defined GL_ES_VERSION_2_0)

    glDepthFunc(GL_LEQUAL);   // The Type Of Depth Testing (Less Or Equal)
    glEnable(GL_DEPTH_TEST);  // Enable Depth Testing

#else
#if (defined GL_VERSION_ES_CM_1_1)
    glClearDepthf(1.0f);                                // Depth Buffer Setup
#else
    glClearDepth(1.0f);  // Depth Buffer Setup
#endif
    glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing (Less Or Equal)
    glEnable(GL_DEPTH_TEST);                            // Enable Depth Testing
    glShadeModel(GL_SMOOTH);                            // Select Smooth Shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Set Perspective Calculations To Most Accurate

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Set Line Antialiasing
    glEnable(GL_LINE_SMOOTH);                // Enable it!
    glEnable(GL_TEXTURE_2D);

#endif

    glEnable(GL_CULL_FACE);  // do not calculate inside of poly's
    glFrontFace(GL_CCW);     // counter clock-wise polygons are out

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_SCISSOR_TEST);  // Enable Clipping

    JGECreateDefaultBindings();

    if (!InitGame()) {
        WGE_LOG_FATAL("Could not init the game");
        return false;
    }

    OnResize(window_w, window_h);

    return true;
};

int main(int argc, char* argv[]) {
    g_launcher = new JGameLauncher();

    u32 flags = g_launcher->GetInitFlags();

    if ((flags & JINIT_FLAG_ENABLE3D) != 0) {
        JRenderer::Set3DFlag(true);
    }

    g_SdlApp = new SdlApp();

    int result = g_SdlApp->OnExecute();

    if (g_launcher) delete g_launcher;
    if (g_SdlApp) delete g_SdlApp;

    DestroyGame();
    return result;
}
