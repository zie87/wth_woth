
#########################################################################
# 3rd
#########################################################################


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

    add_library(old_giflib INTERFACE)
    target_include_directories(old_giflib INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
    set(GIFLIB4_LIBRARY "${DEPENDENCIES_LIBRARY_DIR}/libgif.a")
    target_link_libraries(old_giflib INTERFACE ${GIFLIB4_LIBRARY})
    add_library(WotH::gif ALIAS old_giflib)

    add_library(old_ft INTERFACE)
    target_include_directories(old_ft INTERFACE "${DEPENDENCIES_INCLUDE_DIR}/freetype2")
    set(FREETYPE2_LIBRARY "${DEPENDENCIES_LIBRARY_DIR}/libfreetype.a")
    target_link_libraries(old_ft INTERFACE ${FREETYPE2_LIBRARY})
    add_library(WotH::freetype ALIAS old_ft)

    find_package(ZLIB REQUIRED)
endif()

if(DEFINED WIN32)
    set(DEPENDENCIES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/include/")
    set(DEPENDENCIES_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib/")
    set(DEPENDENCIES_SHARED_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/dll/")

    add_library(png_old INTERFACE)
    target_include_directories(png_old INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
    find_library(PNG_LIBRARY NAMES libpng libpngd PATHS "${DEPENDENCIES_LIBRARY_DIR}" "${DEPENDENCIES_SHARED_LIBRARY_DIR}")
    target_link_libraries(png_old INTERFACE ${PNG_LIBRARY})
    add_library(WotH::png ALIAS png_old)

    add_library(old_giflib INTERFACE)
    target_include_directories(old_giflib INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
    find_library(GIFLIB4_LIBRARY NAMES giflib giflib4 HINTS "${DEPENDENCIES_LIBRARY_DIR}" "${DEPENDENCIES_SHARED_LIBRARY_DIR}")
    target_link_libraries(old_giflib INTERFACE ${GIFLIB4_LIBRARY})
    add_library(WotH::gif ALIAS old_giflib)

    add_library(jpeg_alias INTERFACE)
    target_include_directories(jpeg_alias INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")    
    find_library(JPEG_LIBRARIES NAMES libjpeg-static-mt libjpeg-static-mt-debug HINTS "${DEPENDENCIES_LIBRARY_DIR}" "${DEPENDENCIES_SHARED_LIBRARY_DIR}")
    target_link_libraries(jpeg_alias INTERFACE ${JPEG_LIBRARIES})
    add_library(WotH::jpeg ALIAS jpeg_alias)

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

    add_library(zlib_alias INTERFACE)
    target_include_directories(zlib_alias INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")    
    find_library(ZLIB_LIBRARIES NAMES zlib1 zlib1d zdll zlibd HINTS "${DEPENDENCIES_LIBRARY_DIR}" "${DEPENDENCIES_SHARED_LIBRARY_DIR}")
    target_link_libraries(zlib_alias INTERFACE ${ZLIB_LIBRARIES})
    add_library(ZLIB::ZLIB ALIAS zlib_alias)

    add_library(bthread_alias INTERFACE)
    target_include_directories(bthread_alias INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../Boost")    
    find_library(BTHREAD_LIBRARIES 
        NAMES libboost-thread-vc90-mt-1_44 libboost-thread-vc90-mt-gd-1_44 libboost-thread-vc100-mt-1_44 libboost-thread-vc100-mt-gd-1_44 
        HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../Boost/lib")
    target_link_libraries(bthread_alias INTERFACE ${BTHREAD_LIBRARIES})
    add_library(Boost::thread ALIAS bthread_alias)
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
        WotH::gif
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

if(DEFINED PSP)
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
endif()

if(DEFINED WIN32)
target_sources(jge300
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SDLmain.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSfx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JGfx.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/JSocket.cpp
    )
endif()

set_target_properties(jge300
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS YES
)
target_compile_options(jge300 PRIVATE "$<$<CONFIG:DEBUG>:-ggdb3>")

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
        WotH::gif
        WotH::freetype
        ZLIB::ZLIB
    PRIVATE
        woth_options
)


if(DEFINED WIN32)
    set(JGE_EXTRAS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../mtg/extra")
    target_include_directories(jge300 PUBLIC "${JGE_EXTRAS_INCLUDE_DIR}")
    target_link_libraries(jge300 PUBLIC WotH::fmod Boost::thread)
endif()

add_library(WotH::jge ALIAS jge300)
