add_executable(woth_testsuite)
target_link_libraries(woth_testsuite PRIVATE WotH::wth WotH::options)
target_sources(woth_testsuite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/main_testsuite.cpp)
