#ifndef WOTH_UNITY_UNITYCONFIG_H
#define WOTH_UNITY_UNITYCONFIG_H

#if defined(WOTH_PLATFORM_PSP)

#include <pspdebug.h>

#define UNITY_OUTPUT_CHAR(a) (void)pspDebugScreenPrintf("%c", (char)a);
//#define UNITY_OUTPUT_START()    RS232_config(115200,1,8,0)
//#define UNITY_OUTPUT_FLUSH()    RS232_flush()
//#define UNITY_OUTPUT_COMPLETE() RS232_close()

#endif

#endif // WOTH_UNITY_UNITYCONFIG_H
