set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "ppc")
set(CMAKE_CROSSCOMPILING TRUE)
set(BUILD_SHARED_LIBS OFF)

if(NOT DEFINED ENV{DEVKITPRO})
   message(FATAL_ERROR "You must have defined DEVKITPRO before calling cmake.")
endif()

set(DEVKITPRO $ENV{DEVKITPRO})

if(NOT DEFINED ENV{DEVKITPPC})
   set(DEVKITPPC $ENV{DEVKITPRO}/devkitPPC)
else()
   set(DEVKITPPC $ENV{DEVKITPPC})
endif()

set(CMAKE_FIND_ROOT_PATH "${DEVKITPPC}"
                         "${DEVKITPPC}/powerpc-eabi"
                         "${DEVKITPRO}"
                         "${DEVKITPRO}/tools"
                         "${DEVKITPRO}/portlibs/wii"
                         "${DEVKITPRO}/portlibs/ppc")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(PKG_CONFIG_EXECUTABLE NAMES powerpc-eabi-pkg-config HINTS "${DEVKITPRO}/portlibs/wii/bin")
if (NOT PKG_CONFIG_EXECUTABLE)
   message(WARNING "Could not find powerpc-eabi-pkg-config: try installing wii-pkg-config")
endif()

find_program(DEVKITPPC_GCC NAMES powerpc-eabi-gcc HINTS "${DEVKITPPC}/bin")
if(NOT DEVKITPPC_GCC)
   message(FATAL_ERROR "Could not find powerpc-eabi-gcc")
endif()

find_program(DEVKITPPC_GPP NAMES powerpc-eabi-g++ HINTS "${DEVKITPPC}/bin")
if(NOT DEVKITPPC_GPP)
   message(FATAL_ERROR "Could not find powerpc-eabi-g++")
endif()

find_program(DEVKITPPC_LD NAMES powerpc-eabi-ld HINTS "${DEVKITPPC}/bin")
if(NOT DEVKITPPC_LD)
   message(FATAL_ERROR "Could not find powerpc-eabi-ld")
endif()

find_program(DEVKITPPC_AR NAMES powerpc-eabi-ar HINTS "${DEVKITPPC}/bin")
if(NOT DEVKITPPC_AR)
   message(FATAL_ERROR "Could not find powerpc-eabi-ar")
endif()

find_program(DEVKITPPC_STRIP NAMES powerpc-eabi-strip HINTS "${DEVKITPPC}/bin")
if(NOT DEVKITPPC_STRIP)
   message(FATAL_ERROR "Could not find powerpc-eabi-strip")
endif()

set(CMAKE_ASM_COMPILER     "${DEVKITPPC_GCC}"   CACHE PATH "")
set(CMAKE_C_COMPILER       "${DEVKITPPC_GCC}"   CACHE PATH "")
set(CMAKE_CXX_COMPILER     "${DEVKITPPC_GPP}"   CACHE PATH "")
set(CMAKE_LINKER           "${DEVKITPPC_LD}"    CACHE PATH "")
set(CMAKE_AR               "${DEVKITPPC_AR}"    CACHE PATH "")
set(CMAKE_STRIP            "${DEVKITPPC_STRIP}" CACHE PATH "")


set(WII_C_FLAGS            "-mcpu=750 -mrvl -meabi -mhard-float -Wl,-q -DGEKKO -D__WII__")
set(CMAKE_C_FLAGS          "${WII_C_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS        "${WII_C_FLAGS}" CACHE STRING "")
set(CMAKE_ASM_FLAGS        "${WII_C_FLAGS}" CACHE STRING "")


set(WII TRUE BOOL)

if(NOT TARGET wii_interface)
    add_library(wii_interface INTERFACE)
    add_library(wii::interface ALIAS wii_interface)
endif()
