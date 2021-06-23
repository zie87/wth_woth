set(TEST_NAME "wge_unittests")

add_executable(${TEST_NAME} 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math_tests.cpp
)

target_link_libraries(${TEST_NAME} PUBLIC WotH::wge WotH::wtest)
set_target_properties(${TEST_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(DEFINED PSP)
    create_pbp_file(TARGET ${TEST_NAME} 
                TITLE "WGE unitests" 
                ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/icon.png
                BACKGROUND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/pic1.png
                PREVIEW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/pic0.png
               )
else()
    add_test(tests_wge ${TEST_NAME})
endif()
