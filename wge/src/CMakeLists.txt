
list(APPEND WGE_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/video/image_loader.cpp
)

if(DEFINED WOTH_PSP_BUILD)
    list(APPEND WGE_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/time_psp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thread/mutex_psp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thread/thread_psp.cpp
        )
elseif(DEFINED WOTH_SDL_BUILD)
    list(APPEND WGE_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/time_sdl.cpp
        )
else()
    list(APPEND WGE_SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/time_default.cpp
        )
endif()

add_library(wge STATIC ${WGE_SOURCES})
target_compile_features(wge PUBLIC cxx_std_14)
target_include_directories(wge PUBLIC ${WOTH_WGE_ROOT_DIR}/include)
target_link_libraries(wge PRIVATE woth_options)

if(NOT DEFINED WOTH_PSP_BUILD)
    find_package(Threads REQUIRED)
    target_link_libraries(wge PUBLIC Threads::Threads) 
endif()


add_library(WotH::wge ALIAS wge)
