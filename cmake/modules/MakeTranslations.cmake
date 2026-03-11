#
# License: GPL-2 or later
# Copyright (c) 2026 Nick Egorrov
#
#[==============================[
Creating, updating, compiling and installing translations.


Synopsis
------------

    make_translations(
        name
        [VERBOSE]
        TARGETS target [...]
        LANGUAGES lang_id [...]
        [DESTINATION install_dir]
    )


Options
------------

name
    Specifies the base name of the translation files.
TARGETS target
    List of target containing the source files that need to be translated.
LANGUAGES lang_id
    List of language identifiers for which translations are made.
DESTINATION install_dir
    Optionally, the directory for installing *.qm files. The purpose is the same
    as in the install() function. If not specified, installation will not occur.
VERBOSE
    Optionally enables debug printing.


Details
------------

The function creates two targets: `translations` and `update-translations`. The
`translations` target is run automatically when building the project, while the
`update-translations` target must be run manually when needed.

The `update-translations` target creates and updates `TS` files in the directory
where the corresponding CMakeLists.txt is located, i.e., in the source tree.

The `translations` target creates `QM` files in the corresponding directory of
the build tree.

This function works with Qt4, Qt5, and Qt6.


Example
------------

    make_translations(
        doublecontact
        TARGETS doublecontact quazip
        LANGUAGES de en ru
    )
#]==============================]
function(make_translations ARG_NAME)
    cmake_parse_arguments(ARG "VERBOSE" "DESTINATION" "LANGUAGES;TARGETS" ${ARGN})

    macro(print)
        if (ARG_VERBOSE)
            message(${ARGN})
        endif ()
    endmacro()

    macro(error)
        message(WARNING "${ARGN} Generation of translations will be skipped.")
        return()
    endmacro()

    if (NOT QT_LRELEASE_EXECUTABLE)
        print(STATUS "Try to find the lrelease.")

        if (Qt5_FOUND)
            find_package(${QT} QUIET COMPONENTS LinguistTools)
            set(QT_LUPDATE_EXECUTABLE Qt5::lupdate)
            set(QT_LRELEASE_EXECUTABLE Qt5::lrelease)
        elseif (Qt6_FOUND)
            find_package(${QT} QUIET COMPONENTS LinguistTools)
            set(QT_LUPDATE_EXECUTABLE Qt6::lupdate)
            set(QT_LRELEASE_EXECUTABLE Qt6::lrelease)
        endif ()
    endif ()

    if (NOT QT_LRELEASE_EXECUTABLE)
        error("The lrelease was not found.")
    endif ()

    if (NOT ARG_TARGETS)
        error("The TARGETS options not specified.")
    endif ()

    if (NOT ARG_LANGUAGES)
        error("The LANGUAGES options not specified.")
    endif ()

    set(QT_LUPDATE_EXECUTABLE ${QT_LUPDATE_EXECUTABLE} PARENT_SCOPE)
    set(QT_LRELEASE_EXECUTABLE ${QT_LRELEASE_EXECUTABLE} PARENT_SCOPE)

    print("  MakeTranslations:NAME: ${ARG_NAME}")

    # Create a lists of the TS and QM files.
    # Add a custom command for the each TS file.
    foreach (_lang ${ARG_LANGUAGES})
        print("  MakeTranslations:LANG: ${_lang}")
        set(CAT_NAME ${ARG_NAME}_${_lang})
        set(TS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${CAT_NAME}.ts)
        list(APPEND TS_LIST ${TS_FILE})
        set(QM_FILE ${CMAKE_CURRENT_BINARY_DIR}/${CAT_NAME}.qm)
        list(APPEND QM_LIST ${QM_FILE})
        add_custom_command(OUTPUT ${QM_FILE}
            COMMAND ${QT_LRELEASE_EXECUTABLE} -silent ${TS_FILE} -qm ${QM_FILE}
            DEPENDS ${TS_FILE}
            COMMENT "Generate ${CAT_NAME}.qm")
    endforeach ()

    if (ARG_DESTINATION)
        install(FILES ${QM_LIST} DESTINATION ${ARG_DESTINATION})
    endif ()

    # Add targets to release translations.
    if (NOT TARGET translations)
        add_custom_target(translations ALL)
    endif ()

    add_custom_target(translations-${ARG_NAME} DEPENDS ${QM_LIST})
    add_dependencies(translations translations-${ARG_NAME})

    # Collect the sources that need to be translated.
    foreach (_target ${ARG_TARGETS})
        print("  MakeTranslations:TARGET: ${_target}")
        get_target_property(_source_dir ${_target} SOURCE_DIR)
        get_target_property(_source_list ${_target} SOURCES)

        foreach (_source ${_source_list})
            file(RELATIVE_PATH SRC_REL ${CMAKE_CURRENT_SOURCE_DIR} ${_source_dir}/${_source})
            list(APPEND SRC_LIST ${SRC_REL})
        endforeach ()
    endforeach ()

    # Add targets to update translations.
    if (NOT TARGET update-translations)
        add_custom_target(update-translations)
    endif ()

    set(UPDATE_TARGET update-translations-${ARG_NAME})
    add_custom_target(${UPDATE_TARGET}
        COMMAND ${QT_LUPDATE_EXECUTABLE} ${SRC_LIST} -silent -ts ${TS_LIST}
        DEPENDS ${SRC_LIST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Update ${ARG_NAME}_*.ts")
    add_dependencies(update-translations ${UPDATE_TARGET})
endfunction()
