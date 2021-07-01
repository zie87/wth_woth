set(DEPENDENCIES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set(DEPENDENCIES_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

add_library(fmod INTERFACE)
target_include_directories(fmod INTERFACE "${DEPENDENCIES_INCLUDE_DIR}")
find_library(
    FMOD_LIBRARY
    NAMES fmodvc fmod
    HINTS "${DEPENDENCIES_LIBRARY_DIR}"
)
target_link_libraries(fmod INTERFACE ${FMOD_LIBRARY})
target_compile_definitions(fmod INTERFACE "WITH_FMOD")

add_library(WotH::fmod ALIAS fmod)
