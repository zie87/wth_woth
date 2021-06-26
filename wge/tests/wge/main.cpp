#if defined(WOTH_PLATFORM_PSP)
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>

/* Define the module info section */
PSP_MODULE_INFO("WGE_UNITTEST", 0, 1, 1);

/* Exit callback */
int exit_callback(int arg1, int arg2, void* common) {
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

#else

void start_sys() {}
void close_sys() {}

#endif

#include <wtest/wtest.hpp>

static void RunAllTests() {
    RUN_TEST_GROUP(WgeMath);
    RUN_TEST_GROUP(WgeVideoImageLoader);
}

int main(int argc, const char* argv[]) {
    start_sys();
    const auto return_value = UnityMain(argc, argv, RunAllTests);
    close_sys();
    return return_value;
}
