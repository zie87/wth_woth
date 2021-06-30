
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" AND DEFINED UNIX)
    add_subdirectory(testsuite)
endif()
