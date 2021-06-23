string(TOLOWER ${CMAKE_BUILD_TYPE} ANDROID_BUILD_DIR)

set(ANDROID_DIR ${CMAKE_SOURCE_DIR}/cmake/android)
set(EXTERN_NDK_DIR ${CMAKE_CURRENT_SOURCE_DIR})    

set(DISTRIBUTION_DIR ${ANDROID_DIR}/distribution/android/SDL/intermediates/ndkBuild)
set(EXTERN_SDL_DIR ${EXTERN_NDK_DIR}/SDL PARENT_SCOPE)

# add_library(sdl_interface INTERFACE)
# target_include_directories(sdl_interface INTERFACE ${EXTERN_NDK_DIR}/SDL/include)
# target_link_libraries(sdl_interface INTERFACE ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL.so)
# add_library(SDL::SDL ALIAS sdl_interface)

add_library(SDL::SDL SHARED IMPORTED GLOBAL)
set_target_properties(SDL::SDL PROPERTIES
    IMPORTED_LOCATION ${DISTRIBUTION_DIR}/${ANDROID_BUILD_DIR}/obj/local/${ANDROID_ABI}/libSDL.so
)
set_target_properties(SDL::SDL PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${EXTERN_NDK_DIR}/SDL/include
)


set(WAGIC_JPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libjpeg")
list(
  APPEND JPEG_SOURCES
         "${WAGIC_JPEG_DIR}/jcapimin.c"
         "${WAGIC_JPEG_DIR}/jcapistd.c"
         "${WAGIC_JPEG_DIR}/jccoefct.c"
         "${WAGIC_JPEG_DIR}/jccolor.c"
         "${WAGIC_JPEG_DIR}/jcdctmgr.c"
         "${WAGIC_JPEG_DIR}/jchuff.c"
         "${WAGIC_JPEG_DIR}/jcinit.c"
         "${WAGIC_JPEG_DIR}/jcmainct.c"
         "${WAGIC_JPEG_DIR}/jcmarker.c"
         "${WAGIC_JPEG_DIR}/jcmaster.c"
         "${WAGIC_JPEG_DIR}/jcomapi.c"
         "${WAGIC_JPEG_DIR}/jcparam.c"
         "${WAGIC_JPEG_DIR}/jcphuff.c"
         "${WAGIC_JPEG_DIR}/jcprepct.c"
         "${WAGIC_JPEG_DIR}/jcsample.c"
         "${WAGIC_JPEG_DIR}/jdapimin.c"
         "${WAGIC_JPEG_DIR}/jdapistd.c"
         "${WAGIC_JPEG_DIR}/jdatadst.c"
         "${WAGIC_JPEG_DIR}/jdatasrc.c"
         "${WAGIC_JPEG_DIR}/jdcoefct.c"
         "${WAGIC_JPEG_DIR}/jdcolor.c"
         "${WAGIC_JPEG_DIR}/jddctmgr.c"
         "${WAGIC_JPEG_DIR}/jdhuff.c"
         "${WAGIC_JPEG_DIR}/jdinput.c"
         "${WAGIC_JPEG_DIR}/jdmainct.c"
         "${WAGIC_JPEG_DIR}/jdmarker.c"
         "${WAGIC_JPEG_DIR}/jdmaster.c"
         "${WAGIC_JPEG_DIR}/jdmerge.c"
         "${WAGIC_JPEG_DIR}/jdphuff.c"
         "${WAGIC_JPEG_DIR}/jdpostct.c"
         "${WAGIC_JPEG_DIR}/jdsample.c"
         "${WAGIC_JPEG_DIR}/jerror.c"
         "${WAGIC_JPEG_DIR}/jfdctflt.c"
         "${WAGIC_JPEG_DIR}/jfdctfst.c"
         "${WAGIC_JPEG_DIR}/jfdctint.c"
         "${WAGIC_JPEG_DIR}/jidctflt.c"
         "${WAGIC_JPEG_DIR}/jidctfst.c"
         "${WAGIC_JPEG_DIR}/jidctint.c"
         "${WAGIC_JPEG_DIR}/jmemmgr.c"
         "${WAGIC_JPEG_DIR}/jmemnobs.c"
         "${WAGIC_JPEG_DIR}/jquant1.c"
         "${WAGIC_JPEG_DIR}/jquant2.c"
         "${WAGIC_JPEG_DIR}/jutils.c")

add_library(jpeg STATIC ${JPEG_SOURCES})
target_include_directories(jpeg PUBLIC "${WAGIC_JPEG_DIR}")
target_compile_options(jpeg PUBLIC "-D__ANDROID__" "-DUNICODE")
add_library(JPEG::JPEG ALIAS jpeg)

set(WAGIC_PNG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libpng/jni")
list(
  APPEND PNG_SOURCES
        "${WAGIC_PNG_DIR}/png.c"
        "${WAGIC_PNG_DIR}/pngerror.c"
        "${WAGIC_PNG_DIR}/pngget.c"
        "${WAGIC_PNG_DIR}/pngmem.c"
        "${WAGIC_PNG_DIR}/pngpread.c"
        "${WAGIC_PNG_DIR}/pngread.c"
        "${WAGIC_PNG_DIR}/pngrio.c"
        "${WAGIC_PNG_DIR}/pngrtran.c"
        "${WAGIC_PNG_DIR}/pngrutil.c"
        "${WAGIC_PNG_DIR}/pngset.c"
        "${WAGIC_PNG_DIR}/pngtest.c"
        "${WAGIC_PNG_DIR}/pngtrans.c"
        "${WAGIC_PNG_DIR}/pngwio.c"
        "${WAGIC_PNG_DIR}/pngwrite.c"
        "${WAGIC_PNG_DIR}/pngwtran.c"
        "${WAGIC_PNG_DIR}/pngwutil.c")

find_package(ZLIB REQUIRED)
add_library(ndkpng STATIC ${PNG_SOURCES})
target_include_directories(ndkpng PUBLIC "${WAGIC_PNG_DIR}")
target_compile_options(ndkpng PUBLIC "-D__ANDROID__" "-DUNICODE")
target_link_libraries(ndkpng PUBLIC ZLIB::ZLIB)
add_library(PNG::PNG ALIAS ndkpng)

