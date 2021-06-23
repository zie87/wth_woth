#if defined(WOTH_PLATFORM_PSP)
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>

/* Define the module info section */
PSP_MODULE_INFO("WGE_UNITTEST", 0, 1, 1);

int done = 0;

/* Exit callback */
int exit_callback(int arg1, int arg2, void* common) {
    done = 1;
    return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void* argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);

    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();

    return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void) {
    const auto thread_id = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thread_id >= 0) {
        sceKernelStartThread(thread_id, 0, 0);
    }

    return thread_id;
}

void start_sys() {
    /* Define the main thread's attribute value (optional) */
    PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
    // PSP_HEAP_SIZE_MAX();
    PSP_HEAP_SIZE_KB(-256);

    SetupCallbacks();
    pspDebugScreenInit();
    pspDebugScreenSetXY(0, 0);
}

void close_sys() {
    sceDisplayWaitVblankStart();

    sceKernelSleepThread();
    sceKernelExitGame();
}

#elif defined(WOTH_PLATFORM_WII)

#include <stdlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

void start_sys() {
    VIDEO_Init();
    PAD_Init();

    auto* rmode = VIDEO_GetPreferredMode(NULL);

    auto* xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();
}

void close_sys() {
    while (1) {

        VIDEO_WaitVSync();
        PAD_ScanPads();

        int buttonsDown = WPAD_ButtonsDown(0);

        if (buttonsDown & WPAD_BUTTON_HOME) {
            exit(0);
        }
    }
}

#elif defined(WOTH_PLATFORM_N3DS)

#include <3ds.h>

void start_sys() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
}
void close_sys() {
    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    gfxExit();
}

#else

void start_sys() {}
void close_sys() {}

#endif

extern "C" {
#include <unity_fixture.h>
}

static void RunAllTests() {
    RUN_TEST_GROUP(WgeMath);
}

int main(int argc, const char* argv[]) {
    start_sys();
    const auto return_value = UnityMain(argc, argv, RunAllTests);
    close_sys();
    return return_value;
}
