#include "GameOptions.h"
#include "MTGDeck.h"
#include "TestSuiteAI.h"

#include <JApp.h>
#include <JGE.h>
#include <JFileSystem.h>
#include <JGameLauncher.h>

#include <wge/log.hpp>
#include <wge/time.hpp>
#include <wge/memory.hpp>

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class WagicWrapper {
public:
    WagicWrapper();
    virtual ~WagicWrapper();

private:
    JGE* m_engine;
    JApp* m_app;

    wge::unique_ptr<JGameLauncher> m_launcher;
};

bool JGEToggleFullscreen() { return true; }

void JGECreateDefaultBindings() {}

WagicWrapper::WagicWrapper() : m_engine(nullptr), m_app(nullptr), m_launcher(std::make_unique<JGameLauncher>()) {
    JGECreateDefaultBindings();

    m_engine = JGE::GetInstance();
    m_app    = m_launcher->GetGameApp();
    m_app->Create();
    m_engine->SetApp(m_app);
}

WagicWrapper::~WagicWrapper() {
    if (m_engine) {
        m_engine->SetApp(nullptr);
    }

    if (m_app) {
        m_app->Destroy();
        delete m_app;
        m_app = nullptr;
    }

    JGE::Destroy();
    m_engine = nullptr;
}

int main(int argc, char** argv) {

    // Initialise the video system
    VIDEO_Init();

    // This function initialises the attached controllers
    WPAD_Init();

    // Obtain the preferred video mode from the system
    // This will correspond to the settings in the Wii menu
    GXRModeObj* rmode = VIDEO_GetPreferredMode(NULL);

    // Allocate memory for the display in the uncached region
    void* xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    // Initialise the console, required for printf
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    // Set up the video registers with the chosen mode
    VIDEO_Configure(rmode);

    // Tell the video hardware where our display memory is
    VIDEO_SetNextFramebuffer(xfb);

    // Make the display visible
    VIDEO_SetBlack(FALSE);

    // Flush the video register changes to the hardware
    VIDEO_Flush();

    // Wait for Video setup to complete
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // The console understands VT terminal escape codes
    // This positions the cursor on row 2, column 0
    // we can use variables for this with format codes too
    // e.g. printf ("\x1b[%d;%dH", row, column );
    printf("\x1b[2;0H");

    if (!fatInitDefault()) {
        printf("fatInitDefault failure: terminating\n");
        return -1;
    }

    static const std::string base_dir = "sd:/apps/wagic/";
    static const std::string user_dir = base_dir + "User/";
    static const std::string res_dir  = base_dir + "Res/";
    JFileSystem::init(user_dir, res_dir);
    auto* filesystem = JFileSystem::GetInstance();

    auto wagic_core = std::make_unique<WagicWrapper>();

    MTGCollection()->loadFolder("sets/primitives/");
    MTGCollection()->loadFolder("sets/", "_cards.dat");
    options.reloadProfile();

    TestSuite testSuite("test/_tests.txt");

    WGE_LOG_INFO("system root: {}", filesystem->GetSystemRoot());
    WGE_LOG_INFO("  user root: {}", filesystem->GetUserRoot());

    const auto start_time = wge::clock::now();
    const auto result     = testSuite.run();
    const auto end_time   = wge::clock::now();
    const auto elapsed    = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();

    WGE_LOG_INFO("done: failed test: {} out of {} total needed_time: {} seconds", result,
                 testSuite.nbTests + testSuite.nbAITests, elapsed);

    while (1) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        // this is a "one shot" state which will not fire again until the button has been released
        u32 pressed = WPAD_ButtonsDown(0);

        // We return to the launcher application via exit
        if (pressed & WPAD_BUTTON_HOME) exit(0);

        // Wait for the next frame
        VIDEO_WaitVSync();
    }

    return 0;
}
