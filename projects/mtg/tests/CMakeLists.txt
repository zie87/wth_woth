if(DEFINED UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(testsuite)
endif()
