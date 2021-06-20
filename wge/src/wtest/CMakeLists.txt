
add_library(wtest STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/unity.c
)

target_include_directories(wtest 
    PUBLIC 
        ${WOTH_WGE_ROOT_DIR}/include
        ${WOTH_WGE_ROOT_DIR}/include/wtest/detail
)

target_link_libraries(wtest PRIVATE woth_options)
target_compile_definitions(wtest PUBLIC "UNITY_INCLUDE_CONFIG_H")

add_library(WotH::wtest ALIAS wtest)
