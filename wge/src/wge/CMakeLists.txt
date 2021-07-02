add_library(wge STATIC)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

if(NOT TARGET fmt::fmt AND NOT TARGET fmt)
    find_package(fmt REQUIRED)
endif()

target_sources(
    wge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/video/image_loader.cpp ${CMAKE_CURRENT_SOURCE_DIR}/debug/log_core.cpp
)

if(DEFINED WOTH_PSP_BUILD)
    target_sources(
        wge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/time_psp.cpp ${CMAKE_CURRENT_SOURCE_DIR}/thread/mutex_psp.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_psp.cpp
    )
elseif(DEFINED WOTH_SDL_BUILD)
    find_package(SDL2 REQUIRED)
    target_sources(wge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/time_sdl.cpp)
    target_link_libraries(wge PUBLIC SDL2::SDL2)
else()
    target_sources(wge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/time_default.cpp)
endif()

if(NOT DEFINED WOTH_PSP_BUILD)
    find_package(mpg123 CONFIG )
    
    if(NOT TARGET MPG123::libmpg123)
        find_package(PkgConfig REQUIRED) 
        pkg_check_modules(PC_MPG123 QUIET libmpg123)

        find_path(MPG123_INCLUDE_DIR mpg123.h HINTS ${PC_MPG123_INCLUDEDIR} ${PC_MPG123_INCLUDE_DIRS})
        find_library(MPG123_LIBRARY NAMES mpg123 mpg123-0 HINTS ${PC_MPG123_LIBDIR} ${PC_MPG123_LIBRARY_DIRS})

        set(MPG123_LIBRARIES ${MPG123_LIBRARY})
        set(MPG123_INCLUDE_DIRS ${MPG123_INCLUDE_DIR})

        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(MPG123 DEFAULT_MSG MPG123_LIBRARY MPG123_INCLUDE_DIR)
        mark_as_advanced(MPG123_INCLUDE_DIR MPG123_LIBRARY)

        add_library(mpg123 INTERFACE)
        target_include_directories(mpg123 INTERFACE ${MPG123_INCLUDE_DIR})
        target_link_libraries(mpg123 INTERFACE ${MPG123_LIBRARY})
        add_library(MPG123::libmpg123 ALIAS mpg123)
    endif()

    target_link_libraries(wge PRIVATE MPG123::libmpg123)
endif()


target_compile_features(wge PUBLIC cxx_std_17)
target_include_directories(wge PUBLIC ${WOTH_WGE_ROOT_DIR}/include)
target_link_libraries(
    wge
    PUBLIC fmt::fmt WotH::options
    PRIVATE JPEG::JPEG PNG::PNG WotH::warnings
)

if(NOT DEFINED WOTH_PSP_BUILD)
    find_package(Threads REQUIRED)
    target_link_libraries(wge PUBLIC Threads::Threads)
endif()

add_library(WotH::wge ALIAS wge)
