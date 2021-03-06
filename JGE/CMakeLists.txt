# ######################################################################################################################
# 3rd
# ######################################################################################################################
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Freetype REQUIRED)

if(DEFINED WOTH_SDL_BUILD)
    find_package(SDL2 REQUIRED)

    # on ubuntu target is not defined
    if(NOT TARGET SDL2::SDL2)
        add_library(sdl_wrapper INTERFACE)
        target_include_directories(sdl_wrapper INTERFACE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(sdl_wrapper INTERFACE ${SDL2_LIBRARIES})
        add_library(SDL2::SDL2 ALIAS sdl_wrapper)
    endif()
endif()

# ######################################################################################################################
# HGE
# ######################################################################################################################

add_library(hgetools STATIC)

target_sources(
    hgetools
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgecolor.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgedistort.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgefont.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgeparticle.cpp
            # ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgepmanager.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgerect.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/hge/hgevector.cpp
)

target_include_directories(hgetools PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS)

target_link_libraries(
    hgetools
    PUBLIC WotH::wge
    PRIVATE WotH::options
)

if(DEFINED WOTH_SDL_BUILD)
    target_link_libraries(hgetools PRIVATE SDL2::SDL2)
endif()

add_library(WotH::hge ALIAS hgetools)

# ######################################################################################################################
# JGE
# ######################################################################################################################

add_library(jge300 STATIC)

# generic
target_sources(
    jge300
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/JApp.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JGBKFont.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JGE.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JGui.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JLBFont.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JGameObject.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JSpline.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/JAnimator.cpp
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

if(DISABLE_UI)
    target_sources(
        jge300 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSfx_dummy.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JGfx_dummy.cpp
    )
elseif(DEFINED PSP)
    target_sources(
        jge300
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/JSocket.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/JGfx.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/JSfx.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/JAudio.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/JMP3.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder_prx.c
    )
elseif(DEFINED UNIX OR WIN32)
    target_sources(
        jge300 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSfx.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JGfx.cpp
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSocket.cpp
    )
endif()

if(NOT DEFINED WIN32)
    target_compile_options(jge300 PRIVATE "$<$<CONFIG:DEBUG>:-ggdb3>")
endif()

target_include_directories(jge300 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS)

target_link_libraries(
    jge300
    PUBLIC WotH::hge WotH::wge Freetype::Freetype ZLIB::ZLIB
    PRIVATE PNG::PNG WotH::options
)

if(DEFINED WOTH_SDL_BUILD)
    target_link_libraries(jge300 PUBLIC SDL2::SDL2)
endif()

if(DEFINED WIN32 OR DEFINED UNIX)
    find_package(
        OpenGL
        COMPONENTS OpenGL
        REQUIRED
    )
    target_link_libraries(jge300 PUBLIC OpenGL::GL OpenGL::GLU)
endif()

if(DEFINED WIN32)
    set(JGE_EXTRAS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../projects/mtg/extra")
    target_include_directories(jge300 PUBLIC "${JGE_EXTRAS_INCLUDE_DIR}")
    target_link_libraries(jge300 PUBLIC WotH::fmod)
endif()

add_library(WotH::jge ALIAS jge300)
