if(DEFINED ANDROID)
    add_subdirectory(ndk)
    set(EXTERN_SDL_DIR ${EXTERN_SDL_DIR} PARENT_SCOPE)
endif()

