# Once done this will define
#  LIBOGC_FOUND - System has ctrulib
#  LIBOGC_INCLUDE_DIRS - The ctrulib include directories
#  LIBOGC_LIBRARIES - The libraries needed to use ctrulib
#
# It also adds an imported target named `wii::ogclib`.
# Linking it is the same as target_link_libraries(target ${LIBOGC_LIBRARIES}) and target_include_directories(target ${LIBOGC_INCLUDE_DIRS})

# DevkitPro paths are broken on windows, so we have to fix those
macro(msys_to_cmake_path MsysPath ResultingPath)
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()

if(NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif()

set(OGCLIB_PATHS $ENV{OGCLIB} libogc ${DEVKITPRO}/libogc)

find_path(LIBOGC_INCLUDE_DIR ogcsys.h
          PATHS ${OGCLIB_PATHS}
          PATH_SUFFIXES include libogc/include )

find_library(LIBOGC_LIBRARY NAMES ogc libogc.a
          PATHS ${OGCLIB_PATHS}
          PATH_SUFFIXES lib lib/wii libogc/lib libogc/lib/wii )

find_library(LIBOGC_BTE_LIBRARY NAMES bte libbte.a
          PATHS ${OGCLIB_PATHS}
          PATH_SUFFIXES lib lib/wii libogc/lib libogc/lib/wii )

find_library(LIBOGC_FAT_LIBRARY NAMES fat libfat.a
          PATHS ${OGCLIB_PATHS}
          PATH_SUFFIXES lib lib/wii libogc/lib libogc/lib/wii )

find_library(LIBOGC_WIIUSE_LIBRARY NAMES wiiuse libwiiuse.a
          PATHS ${OGCLIB_PATHS}
          PATH_SUFFIXES lib lib/wii libogc/lib libogc/lib/wii )

set(LIBOGC_LIBRARIES ${LIBOGC_LIBRARY} ${LIBOGC_WIIUSE_LIBRARY})
set(LIBOGC_INCLUDE_DIRS ${LIBOGC_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBOGC_FOUND to TRUE if all listed variables are TRUE
find_package_handle_standard_args(OGCLIB DEFAULT_MSG LIBOGC_LIBRARY LIBOGC_WIIUSE_LIBRARY LIBOGC_INCLUDE_DIR)

mark_as_advanced(LIBOGC_INCLUDE_DIR LIBOGC_LIBRARY )
if(OGCLIB_FOUND)
    set(OGCLIB ${LIBOGC_INCLUDE_DIR}/..)
    message(STATUS "setting OGCLIB to ${OGCLIB}")

    add_library(wii::ogclib STATIC IMPORTED GLOBAL)
    set_target_properties(wii::ogclib PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LIBOGC_INCLUDE_DIR}
        IMPORTED_LOCATION "${LIBOGC_LIBRARY}"
    )

    add_library(wii::bte STATIC IMPORTED GLOBAL)
    set_target_properties(wii::bte PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LIBOGC_INCLUDE_DIR}
        IMPORTED_LOCATION "${LIBOGC_BTE_LIBRARY}"
    )
    target_link_libraries(wii::bte INTERFACE wii::ogclib)

    add_library(wii::fat STATIC IMPORTED GLOBAL)
    set_target_properties(wii::fat PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LIBOGC_INCLUDE_DIR}
        IMPORTED_LOCATION "${LIBOGC_FAT_LIBRARY}"
    )
    target_link_libraries(wii::fat INTERFACE wii::ogclib)

    add_library(wii::wiiuse STATIC IMPORTED GLOBAL)
    set_target_properties(wii::wiiuse PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${LIBOGC_INCLUDE_DIR}
        IMPORTED_LOCATION "${LIBOGC_WIIUSE_LIBRARY}"
    )
    target_link_libraries(wii::wiiuse INTERFACE wii::bte wii::ogclib)

endif()
