# Create and install translation files.

include(MakeTranslations)

make_translations(
    doublecontact
    TARGETS doublecontact quazip_static S_CORE S_MODEL # contconv
    LANGUAGES de en_GB fr ie nb_NO nl pl pt pt_BR ru_RU uk_UA zh_Hant
    DESTINATION ${APP_I18N_PATH}
)

install(FILES iso639-1.utf8 DESTINATION ${APP_I18N_PATH})

# Needed for localisation when application is launched from the build directory.
file(COPY iso639-1.utf8 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
