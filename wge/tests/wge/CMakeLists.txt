
add_executable(wge_tests 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math_tests.cpp
)

target_link_libraries(wge_tests PUBLIC WotH::wge WotH::wtest)
set_target_properties(wge_tests PROPERTIES LINKER_LANGUAGE CXX)

if(DEFINED PSP)
    create_pbp_file(TARGET wge_tests 
                TITLE "WGE unitests" 
                ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/icon.png
                BACKGROUND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/pic1.png
                PREVIEW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data/pic0.png
               )
endif()
