
#########################################################################
# 3rd
#########################################################################

add_library(png13 INTERFACE)
target_include_directories(png13 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/psp)
target_link_directories(png13 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/psp)
target_link_libraries(png13 INTERFACE png)
add_library(WotH::png ALIAS png13)

add_library(jpg INTERFACE)
target_include_directories(jpg INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/psp)
target_link_directories(jpg INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/psp)
target_link_libraries(jpg INTERFACE jpeg)
add_library(WotH::jpeg ALIAS jpg)


add_library(giflib INTERFACE)
target_include_directories(giflib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/psp)
target_link_directories(giflib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/psp)
target_link_libraries(giflib INTERFACE gif)
add_library(WotH::gif ALIAS giflib)

#########################################################################
# HGE
#########################################################################

add_library(hgetools STATIC)

target_sources(hgetools 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgecolor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgedistort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgefont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgeparticle.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgepmanager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgerect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgevector.cpp
)

target_include_directories(hgetools 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psp/freetype2
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS
)

target_link_libraries(hgetools
    PRIVATE
        woth_options
)

add_library(WotH::hge ALIAS hgetools)

##########################################################################
# JGE
##########################################################################

add_library(jge300 STATIC)

# generic
target_sources(jge300
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JApp.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JGBKFont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JGE.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JGui.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JLBFont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JLogger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JGameObject.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JSpline.cpp src/JAnimator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JResourceManager.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JFileSystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JNetwork.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JParticle.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JParticleEmitter.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JParticleEffect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JParticleSystem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS/zfsystem.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS/ziphdr.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS/zstream.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JSprite.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Vector2D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml/tinystr.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml/tinyxml.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml/tinyxmlparser.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tinyxml/tinyxmlerror.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Encoding.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JTTFont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JMD2Model.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JOBJModel.cpp
    )

# psp
target_sources(jge300
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JSocket.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JGfx.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JSfx.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JAudio.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JMP3.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder_prx.c 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vram.c
    )


set_target_properties(jge300
    PROPERTIES
        CXX_STANDARD 98
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS ON
)
target_compile_options(jge300 PRIVATE "$<$<CONFIG:DEBUG>:-ggdb3>")

target_include_directories(jge300
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Boost
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psp/freetype2
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS
)

find_package(ZLIB REQUIRED)

target_link_libraries(jge300
    PUBLIC
        WotH::hge
        WotH::png
        WotH::jpeg
        WotH::gif
        ZLIB::ZLIB
    PRIVATE
        woth_options
)

add_library(WotH::jge ALIAS jge300)
