#ifndef WOTH_WGE_THREAD_HPP
#define WOTH_WGE_THREAD_HPP

#if defined WOTH_PLATFORM_PSP
    #include "wge/thread/thread_psp.hpp"
    #include "wge/thread/mutex_psp.hpp"
#elif defined WOTH_PLATFORM_WII
    #include "wge/thread/thread_wii.hpp"
    #include "wge/thread/mutex_wii.hpp"
#elif defined WOTH_PLATFORM_N3DS
    #include "wge/thread/thread_n3ds.hpp"
    #include "wge/thread/mutex_n3ds.hpp"
#else
    #include "wge/thread/thread_default.hpp"
    #include "wge/thread/mutex_default.hpp"
#endif

#endif  // WOTH_WGE_THREAD_HPP
