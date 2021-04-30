cmake_minimum_required(VERSION 3.10)

project(wth_woth LANGUAGES C CXX)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WOTH_CMAKE_MODULES_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/modules)
list(APPEND CMAKE_MODULE_PATH ${WOTH_CMAKE_MODULES_PATH})

add_library(woth_options INTERFACE)
target_compile_options(woth_options INTERFACE "$<$<CONFIG:DEBUG>:-D_DEBUG>")
target_compile_options(woth_options INTERFACE "$<$<CONFIG:RELEASE>:-O2>")

if(DEFINED PSP)
    target_link_libraries(woth_options INTERFACE psp::interface)
    target_compile_definitions(woth_options INTERFACE "WOTH_PLATFORM_PSP")
    target_compile_definitions(woth_options INTERFACE "PSP")
    target_compile_definitions(woth_options INTERFACE "DEVHOOK")
    target_compile_definitions(woth_options INTERFACE "PSPFW3XX")
    
    include(psp_tools) 
    set(WOTH_PSP_BUILD ON)
elseif(DEFINED WIN32)
    target_compile_options(woth_options INTERFACE "/wd4005")

    target_compile_definitions(woth_options INTERFACE "WOTH_PLATFORM_WIN32")
    target_compile_definitions(woth_options INTERFACE "WIN32")
    target_compile_definitions(woth_options INTERFACE "_CRT_SECURE_NO_WARNINGS")
    target_compile_definitions(woth_options INTERFACE "NOMINMAX")

    find_package(Threads REQUIRED)
    target_link_libraries(woth_options INTERFACE Threads::Threads)
else() 
    message(FATAL_ERROR "Platform can not be detected!")
endif()

add_subdirectory(wge)
add_subdirectory(JGE)
add_subdirectory(projects)
