cmake_minimum_required(VERSION 3.10)

set(WOTH_CMAKE_MODULES_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${WOTH_CMAKE_MODULES_PATH})

find_package(Git REQUIRED)
include( MunkeiVersionFromGit )

version_from_git(
    GIT_EXECUTABLE ${GIT_EXECUTABLE}
    LOG       ON
    TIMESTAMP "%Y%m%d%H%M%S"
)


project(wth_woth LANGUAGES C CXX VERSION ${VERSION})

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(WOTH_PROJECT_NAME "${CMAKE_PROJECT_NAME}")
set(WOTH_PROJECT_DESCRIPTION "Wagic, the Homebrew?!")


option(DISABLE_UI "disable ui application (only for tests)" OFF)

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CompilerWarnings)

add_library(woth_warnings INTERFACE)
set_project_warnings(woth_warnings)
add_library(WotH::warnings ALIAS woth_warnings)

add_library(woth_options INTERFACE)
target_compile_options(woth_options INTERFACE "$<$<CONFIG:DEBUG>:-DWOTH_DEBUG>")
target_compile_options(woth_options INTERFACE "$<$<CONFIG:DEBUG>:-D_DEBUG>")
target_compile_options(woth_options INTERFACE "$<$<CONFIG:RELEASE>:-O2>")

include(Sanitizers)
enable_sanitizers(woth_options)

include(StaticAnalyzers)

if(NOT DISABLE_UI)
    if(DEFINED PSP)
        set(WOTH_PSP_BUILD ON)
    else()
        target_compile_definitions(woth_options INTERFACE "SDL_CONFIG")
        set(WOTH_SDL_BUILD ON)
    endif()
endif()

if(DEFINED PSP)
    target_link_libraries(woth_options INTERFACE psp::interface)
    target_compile_definitions(woth_options INTERFACE "WOTH_PLATFORM_PSP")
    target_compile_definitions(woth_options INTERFACE "PSP")
    target_compile_definitions(woth_options INTERFACE "DEVHOOK")
    target_compile_definitions(woth_options INTERFACE "PSPFW3XX")
    
    include(PspTools)
    set(WOTH_SYSTEM_NAME "psp")
elseif(DEFINED UNIX)
    target_compile_definitions(woth_options INTERFACE "WOTH_PLATFORM_UNIX")
    target_compile_definitions(woth_options INTERFACE "LINUX")
    set(WOTH_SYSTEM_NAME "linux")
elseif(DEFINED WIN32)
    target_compile_options(woth_options INTERFACE "/wd4005")

    target_compile_definitions(woth_options INTERFACE "WOTH_PLATFORM_WIN32")
    target_compile_definitions(woth_options INTERFACE "WIN32")
    target_compile_definitions(woth_options INTERFACE "NOMINMAX")
    target_compile_definitions(woth_options INTERFACE "__STDC_LIMIT_MACROS")
    target_compile_definitions(woth_options INTERFACE "_CRT_SECURE_NO_WARNINGS")
    set(WOTH_SYSTEM_NAME "win32")
else() 
    message(FATAL_ERROR "Platform can not be detected!")
endif()

set(WOTH_INSTALL_BINDIR .)
set(WOTH_INSTALL_LIBDIR .)
set(WOTH_INSTALL_DATADIR .)

add_library(WotH::options ALIAS woth_options)

enable_testing()
add_subdirectory(wge)
add_subdirectory(JGE)
add_subdirectory(projects)

set(CPACK_PACKAGE_NAME ${WOTH_PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION ${WOTH_PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${CMAKE_PROJECT_VERSION})
set(CPACK_SYSTEM_NAME ${WOTH_SYSTEM_NAME})
set(CPACK_PACKAGE_VENDOR "zie87")
set(CPACK_PACKAGE_CONTACT "zie.develop@gmail.com")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_SYSTEM_NAME}")
set(CPACK_GENERATOR "ZIP")
include (CPack)
