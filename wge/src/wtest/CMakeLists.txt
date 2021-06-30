add_library(wtest STATIC ${CMAKE_CURRENT_SOURCE_DIR}/unity.c ${CMAKE_CURRENT_SOURCE_DIR}/unity_fixture.c)

target_include_directories(wtest PUBLIC ${WOTH_WGE_ROOT_DIR}/include ${WOTH_WGE_ROOT_DIR}/include/wtest/detail)

target_link_libraries(
    wtest
    PUBLIC WotH::wge
    PRIVATE WotH::options
)
target_compile_definitions(wtest PUBLIC "UNITY_FIXTURE_NO_EXTRAS")

if(DEFINED WOTH_PSP_BUILD)
    target_compile_definitions(wtest PUBLIC "UNITY_INCLUDE_CONFIG_H")
endif()

add_library(WotH::wtest ALIAS wtest)
