#ifndef DEBUGROUTINES_H
#define DEBUGROUTINES_H

// dirty, but I get OS header includes this way
#include "JGE.h"

#include <ostream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <sstream>

template <class T>
std::string ToHex(T* pointer) {
    std::ostringstream stream;
    stream << std::hex << std::showbase << std::setfill('0') << std::setw(8) << (uint64_t)pointer;
    return stream.str();
}

#ifdef LINUX
    #define OutputDebugString(val) (std::cerr << val);
#endif

#ifdef _DEBUG
    #if defined(WIN32) || defined(LINUX)

        #ifdef QT_CONFIG
            #define DebugTrace(inString)             \
                {                                    \
                    std::ostringstream stream;       \
                    stream << inString << std::endl; \
                    qDebug(stream.str().c_str());    \
                }
        #else
            #define DebugTrace(inString)                     \
                {                                            \
                    std::ostringstream stream;               \
                    stream << inString << std::endl;         \
                    OutputDebugString(stream.str().c_str()); \
                }
        #endif  // QT_CONFIG
    #endif      // Win32, Linux
#endif          //#ifdef _DEBUG

#if defined(DEBUG)
    #ifndef DebugTrace
        #define DebugTrace(inString) \
            { std::cerr << inString << std::endl; }
    #endif  // DEBUG
#endif

#ifndef DebugTrace
    #define DebugTrace(inString) (void(0))
#endif

#endif  // DEBUGROUTINES_H
