find_program(PSP_FIXUP_IMPORTS_CMD "psp-fixup-imports")
find_program(PSP_PRXGEN_CMD "psp-prxgen")
find_program(PSP_PACK_PBP_CMD "pack-pbp")
find_program(PSP_MKSFO_CMD "mksfo")
find_program(PSP_MKSFOEX_CMD "mksfoex")
find_program(PSP_PRXENC_CMD "PrxEncrypter")

macro(create_pbp_file)
    set(oneValueArgs TARGET TITLE ICON_PATH BACKGROUND_PATH PREVIEW_PATH)
    cmake_parse_arguments("ARG" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # As pack-pbp takes undefined arguments in form of "NULL" string,
    # set each undefined macro variable to such value:
    foreach(arg ${oneValueArgs})
        if (NOT DEFINED ARG_${arg})
            set(ARG_${arg} "NULL")
        endif()
    endforeach()

    add_custom_command(
            TARGET ${ARG_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${ARG_TARGET}>/psp_artifact
            COMMENT "Creating psp_artifact directory."
    )

    add_custom_command(
            TARGET ${ARG_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${ARG_TARGET}>
            $<TARGET_FILE_DIR:${ARG_TARGET}>/psp_artifact
            COMMENT "Copying ELF to psp_arfitact directory."
    )

    add_custom_command(
            TARGET ${ARG_TARGET}
            POST_BUILD COMMAND
            "${PSP_FIXUP_IMPORTS_CMD}" "$<TARGET_FILE_DIR:${ARG_TARGET}>/psp_artifact/${ARG_TARGET}"
            COMMENT "Calling psp-fixup-imports"
    )

    add_custom_command(
            TARGET ${ARG_TARGET}
            POST_BUILD COMMAND
            "${PSP_PRXGEN_CMD}" "$<TARGET_FILE_DIR:${ARG_TARGET}>/psp_artifact/${ARG_TARGET}" "$<TARGET_FILE_DIR:${ARG_TARGET}>/psp_artifact/${ARG_TARGET}.prx"
            COMMENT "Calling psp-prxgen"
    )

    add_custom_command(
            TARGET ${ARG_TARGET}
            POST_BUILD COMMAND
            "${PSP_MKSFOEX_CMD}" "${ARG_TITLE}" "PARAM.SFO"
            COMMENT "Calling mksfoex"
    )

    add_custom_command(
            TARGET ${ARG_TARGET}
            POST_BUILD COMMAND
            "${PSP_PACK_PBP_CMD}" "EBOOT.PBP" "PARAM.SFO" "${ARG_ICON_PATH}" "NULL" "${ARG_PREVIEW_PATH}"
            "${ARG_BACKGROUND_PATH}" "NULL" "$<TARGET_FILE_DIR:${ARG_TARGET}>/psp_artifact/${ARG_TARGET}.prx" "NULL"
            COMMENT "Calling pack-pbp"
    )

    add_custom_command(
            TARGET ${ARG_TARGET}
            POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E cmake_echo_color --cyan "EBOOT.PBP file created."
    )
endmacro()
