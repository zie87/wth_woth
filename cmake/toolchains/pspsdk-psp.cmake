set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_CROSSCOMPILING TRUE)
set(BUILD_SHARED_LIBS FALSE)

if(NOT DEFINED PSPDEV)
    execute_process(COMMAND bash -c "psp-config --pspdev-path" OUTPUT_VARIABLE PSPDEV OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if (NOT DEFINED PSPDEV)
    message(FATAL_ERROR "PSPDEV not defined. Make sure psp-config in your path or pass custom \
                        toolchain location via PSPDEV variable in cmake call.")
endif ()

if(NOT DEFINED PSPSDK)
    set(PSPSDK ${PSPDEV}/psp/sdk)

    if(NOT EXISTS ${PSPSDK})
        execute_process(COMMAND bash -c "psp-config --pspsdk-path" OUTPUT_VARIABLE PSPSDK OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
endif()

if(NOT EXISTS ${PSPSDK})
    message(FATAL_ERROR "couldn't find pspsdk directory (${PSPSDK})")
endif()

if(NOT DEFINED PSP_PREFIX)
    execute_process(COMMAND bash -c "psp-config --psp-prefix" OUTPUT_VARIABLE PSP_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if(NOT EXISTS ${PSP_PREFIX})
    message(FATAL_ERROR "couldn't find psp prefix (${PSP_PREFIX})")
endif()

set(CMAKE_FIND_ROOT_PATH "${PSPSDK};${PSPDEV};${PSP_PREFIX}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(PSP_C_COMPILER "psp-gcc" PATHS ${PSPDEV} PATH_SUFFIXES "bin")
set(CMAKE_C_COMPILER ${PSP_C_COMPILER})

find_program(PSP_CXX_COMPILER "psp-g++" PATHS ${PSPDEV} PATH_SUFFIXES "bin")
set(CMAKE_CXX_COMPILER ${PSP_CXX_COMPILER})

find_program(PSP_STRIP "psp-strip" PATHS ${PSPDEV} PATH_SUFFIXES "bin")
set(CMAKE_STRIP ${PSP_STRIP})

set(PRXSPECS_FILE "${PSPSDK}/lib/prxspecs")
set(LINKFILE_FILE "${PSPSDK}/lib/linkfile.prx")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=${PRXSPECS_FILE} -Wl,-q,-T${LINKFILE_FILE}")

set(PSP_LIBRARIES "-lpspdebug -lpspdisplay -lpspge -lpspctrl -lc -lpspsdk -lc -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpspaudiolib -lpsputility -lpspuser -lpspkernel")
set(CMAKE_C_STANDARD_LIBRARIES "${PSP_LIBRARIES}")
set(CMAKE_CXX_STANDARD_LIBRARIES "-lstdc++ ${PSP_LIBRARIES}")


set(PSP TRUE BOOL)

if(NOT TARGET psp_interface)
    add_library(psp_interface INTERFACE)
    target_include_directories(psp_interface SYSTEM INTERFACE ${PSPDEV}/include)
    target_include_directories(psp_interface SYSTEM INTERFACE ${PSPSDK}/include)

    target_compile_definitions(psp_interface INTERFACE "_PSP_FW_VERSION=371")

    target_compile_options(psp_interface INTERFACE "-G0")

    set(PSP_LINK_LIBS
            pspvram
            psppower 
            pspmpeg 
            pspaudiocodec 
            pspaudiolib 
            pspaudio 
            pspmp3 
            pspgum 
            pspgu 
            psprtc 
            pspfpu 
    )

    target_link_libraries(psp_interface INTERFACE ${PSP_LINK_LIBS} )
    target_link_directories(psp_interface INTERFACE ${PSPSDK}/lib)
    target_link_directories(psp_interface INTERFACE ${PSPDEV}/lib)

    add_library(psp::interface ALIAS psp_interface)
endif()

