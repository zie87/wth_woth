set(WOTH_WGE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(extern EXCLUDE_FROM_ALL)

add_subdirectory(src)
add_subdirectory(tests)
