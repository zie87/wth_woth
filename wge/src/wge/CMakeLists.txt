add_library(wge STATIC)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

if (NOT TARGET fmt::fmt AND NOT TARGET fmt)
    find_package(fmt REQUIRED)
endif ()

target_sources(wge 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/video/image_loader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/debug/log_core.cpp
)

if(DEFINED WOTH_PSP_BUILD)
    target_sources(wge 
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/time_psp.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/thread/mutex_psp.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_psp.cpp
    )
elseif(DEFINED WOTH_SDL_BUILD)
    find_package(SDL2 REQUIRED)
    target_sources(wge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/time_sdl.cpp)
    target_link_libraries(wge PUBLIC SDL2::SDL2)
else()
    target_sources(wge PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/time_default.cpp)
endif()

target_compile_features(wge PUBLIC cxx_std_17)
target_include_directories(wge PUBLIC ${WOTH_WGE_ROOT_DIR}/include)
target_link_libraries(wge PUBLIC fmt::fmt WotH::options PRIVATE JPEG::JPEG PNG::PNG WotH::warnings) 


if(NOT DEFINED WOTH_PSP_BUILD)
    find_package(Threads REQUIRED)
    target_link_libraries(wge PUBLIC Threads::Threads) 
endif()

add_library(WotH::wge ALIAS wge)
