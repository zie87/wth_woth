cmake_minimum_required(VERSION 3.10)

project(wge LANGUAGES C CXX)

set(WOTH_WGE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(extern)
add_subdirectory(src)
