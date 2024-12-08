# Create and install translation files.

cmake_minimum_required(VERSION 3.5)

find_program (LRELEASE_EXE lrelease)

if (LRELEASE_EXE)
    message("-- Found lrelease")
else ()
    message(WARNING "The lrelease was not found. Generation of translations will be skipped.")
    return()
endif ()

set(LANGUAGES de en_GB fr ie nb_NO nl pl pt pt_BR ru_RU uk_UA zh_Hant)

foreach (LANG ${LANGUAGES})
    set(TS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/doublecontact_${LANG}.ts)
    set(QM_FILE ${CMAKE_CURRENT_BINARY_DIR}/doublecontact_${LANG}.qm)
    list(APPEND QM_LIST ${QM_FILE})
    add_custom_command(OUTPUT ${QM_FILE}
        COMMAND ${LRELEASE_EXE} -silent ${TS_FILE} -qm ${QM_FILE}
        DEPENDS ${TS_FILE}
        COMMENT "Generate doublecontact_${LANG}.qm")
    install(FILES ${QM_FILE} DESTINATION ${APP_I18N_PATH})
endforeach ()

add_custom_target(translations ALL DEPENDS ${QM_LIST})
install(FILES iso639-1.utf8 DESTINATION ${APP_I18N_PATH})
# Needed for localisation when application is launched from the build directory.
file(COPY_FILE iso639-1.utf8 ${CMAKE_CURRENT_BINARY_DIR}/iso639-1.utf8)
