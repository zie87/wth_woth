set(WOTH_WGE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)

if(NOT DEFINED ANDROID)
    add_subdirectory(tests)
endif()
