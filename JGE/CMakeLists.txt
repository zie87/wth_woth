
#########################################################################
# 3rd
#########################################################################
find_package(ZLIB REQUIRED)

if(DEFINED PSP)
    set(DEPENDENCIES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/psp")
    set(DEPENDENCIES_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/psp")

    add_library(png_old INTERFACE)
    target_include_directories(png_old INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
    set(PNG_LIBRARY "${DEPENDENCIES_LIBRARY_DIR}/libpng.a")
    target_link_libraries(png_old INTERFACE ${PNG_LIBRARY})
    add_library(WotH::png ALIAS png_old)

    add_library(jpeg_old INTERFACE)
    target_include_directories(jpeg_old INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
    set(JPEG_LIBRARY "${DEPENDENCIES_LIBRARY_DIR}/libjpeg.a")
    target_link_libraries(jpeg_old INTERFACE ${JPEG_LIBRARY})
    add_library(WotH::jpeg ALIAS jpeg_old)

    add_library(old_ft INTERFACE)
    target_include_directories(old_ft INTERFACE "${DEPENDENCIES_INCLUDE_DIR}/freetype2")
    set(FREETYPE2_LIBRARY "${DEPENDENCIES_LIBRARY_DIR}/libfreetype.a")
    target_link_libraries(old_ft INTERFACE ${FREETYPE2_LIBRARY})
    add_library(WotH::freetype ALIAS old_ft)

endif()

if(DEFINED UNIX OR DEFINED WIN32)
    add_library(jpg_alias INTERFACE)
    add_library(WotH::jpeg ALIAS jpg_alias)

    add_library(png_old INTERFACE)
    add_library(WotH::png ALIAS png_old)
endif()

if(NOT DEFINED ANDROID AND DEFINED UNIX)
    find_package(Freetype REQUIRED)
    add_library(ft_alias INTERFACE)
    target_link_libraries(ft_alias INTERFACE Freetype::Freetype)
    add_library(WotH::freetype ALIAS ft_alias)
endif()

if(DEFINED WIN32)
    set(DEPENDENCIES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/include/")
    set(DEPENDENCIES_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/")
    set(DEPENDENCIES_SHARED_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/dll/")

    add_library(old_ft INTERFACE)
    target_include_directories(old_ft INTERFACE "${DEPENDENCIES_INCLUDE_DIR}/freetype2")
    find_library(FREETYPE2_LIBRARY NAMES freetype HINTS "${DEPENDENCIES_LIBRARY_DIR}")
    target_link_libraries(old_ft INTERFACE ${FREETYPE2_LIBRARY})
    add_library(WotH::freetype ALIAS old_ft)

    add_library(fmod INTERFACE)
    target_include_directories(fmod INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
    find_library(FMOD_LIBRARY NAMES fmodvc HINTS "${DEPENDENCIES_LIBRARY_DIR}")
    target_link_libraries(fmod INTERFACE ${FMOD_LIBRARY})
    add_library(WotH::fmod ALIAS fmod)
endif()

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS
)

target_link_libraries(hgetools
    PUBLIC
        WotH::wge
    PRIVATE
        WotH::options
)

if(DEFINED ANDROID)
    target_link_libraries(hgetools PRIVATE SDL::SDL)
elseif(DEFINED UNIX OR DEFINED WIN32)
    target_link_libraries(hgetools PRIVATE SDL2::SDL2)
endif()

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JMD2Model.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JOBJModel.cpp
    )

if(DEFINED PSP)
target_sources(jge300
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JTTFont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JSocket.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JGfx.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JSfx.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JAudio.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JMP3.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/decoder_prx.c 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vram.c
    )
endif()

if(DEFINED ANDROID)
target_sources(jge300
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SDLmain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/android/JSfx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JGfx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSocket.cpp
    )

elseif(DEFINED UNIX OR WIN32)
target_sources(jge300
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/JTTFont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SDL2main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSfx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JGfx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSocket.cpp
    )
endif()

if(NOT DEFINED WIN32)
target_compile_options(jge300 PRIVATE "$<$<CONFIG:DEBUG>:-ggdb3>")
endif()

target_include_directories(jge300
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/zipFS
)

target_link_libraries(jge300
    PUBLIC
        WotH::hge
        WotH::wge
        WotH::png
        WotH::jpeg
        ZLIB::ZLIB
    PRIVATE
        WotH::options
)

if(NOT DEFINED ANDROID)
target_link_libraries(jge300
    PUBLIC
        WotH::freetype
)
endif()

if(DEFINED ANDROID)        
    target_include_directories(jge300 PUBLIC "${CMAKE_SOURCE_DIR}/projects/mtg/include")
    target_link_libraries(jge300 PUBLIC android log EGL GLESv1_CM GLESv2 OpenSLES)
    target_link_libraries(jge300 PUBLIC SDL::SDL)
elseif(DEFINED UNIX OR DEFINED WIN32)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL COMPONENTS OpenGL REQUIRED)

    # on ubuntu target is not defined
    if(NOT TARGET SDL2::SDL2)
        add_library(sdl_wrapper INTERFACE)
        target_include_directories(sdl_wrapper INTERFACE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(sdl_wrapper INTERFACE ${SDL2_LIBRARIES})
        add_library(SDL2::SDL2 ALIAS sdl_wrapper)
    endif()

    target_link_libraries(jge300 PUBLIC SDL2::SDL2 OpenGL::GL OpenGL::GLU )
endif()

if(DEFINED WIN32)
    set(JGE_EXTRAS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../projects/mtg/extra")
    target_include_directories(jge300 PUBLIC "${JGE_EXTRAS_INCLUDE_DIR}")
    target_link_libraries(jge300 PUBLIC WotH::fmod)
endif()

add_library(WotH::jge ALIAS jge300)
