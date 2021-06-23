set(TEST_NAME "jge_unittests")

add_executable(${TEST_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${TEST_NAME} PUBLIC WotH::jge WotH::wtest)

if(DEFINED PSP)
    create_pbp_file(TARGET 
        ${TEST_NAME} 
        TITLE "WtH Unit Tests" 
        ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/icon.png
        BACKGROUND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/pic1.png
        PREVIEW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/pic0.png
    )
else()
    add_test(tests_jge ${TEST_NAME})
endif()
