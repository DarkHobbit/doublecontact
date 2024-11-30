
cmake_minimum_required (VERSION 3.5)

project(DoubleContactApp LANGUAGES CXX)

add_executable(doublecontact)
set_target_properties(doublecontact PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)
target_link_libraries(doublecontact Qt::Core Qt::Widgets Qt::Xml quazip_static S_CORE S_MODEL)

install(TARGETS doublecontact DESTINATION ${APP_BIN_PATH})
install(FILES doublecontact.desktop DESTINATION ${APP_DSK_PATH})

target_sources(doublecontact
    PRIVATE
    # HEADERS
    ################
        mainwindow.h
        contactdialog.h
        phonetypedialog.h
        logwindow.h
        settingsdialog.h
        datedetailsdialog.h
        helpers.h
        comparecontainers.h
        comparedialog.h
        innerfilewindow.h
        multicontactdialog.h
        aboutdialog.h
        languageselectdialog.h
        csvprofiledialog.h
        sortdialog.h
        tagremovedialog.h
        groupdialog.h
    # SOURCES
    ################
        main.cpp
        mainwindow.cpp
        contactdialog.cpp
        phonetypedialog.cpp
        logwindow.cpp
        settingsdialog.cpp
        datedetailsdialog.cpp
        helpers.cpp
        comparecontainers.cpp
        comparedialog.cpp
        innerfilewindow.cpp
        multicontactdialog.cpp
        aboutdialog.cpp
        languageselectdialog.cpp
        csvprofiledialog.cpp
        sortdialog.cpp
        tagremovedialog.cpp
        groupdialog.cpp
    # FORMS
    ################
        mainwindow.ui
        contactdialog.ui
        phonetypedialog.ui
        logwindow.ui
        settingsdialog.ui
        datedetailsdialog.ui
        comparedialog.ui
        multicontactdialog.ui
        aboutdialog.ui
        languageselectdialog.ui
    # RESOURCES
    ################
        doublecontact.qrc
)

if (WITH_CALLS)
    target_compile_definitions(doublecontact PRIVATE WITH_CALLS)
    target_sources(doublecontact
        PRIVATE
            callwindow.h
            callwindow.cpp
            callwindow.ui
    )
endif ()

if (WITH_MESSAGES)
    target_compile_definitions(doublecontact PRIVATE WITH_MESSAGES)
    target_sources(doublecontact
        PRIVATE
            messagewindow.h
            messagewindow.cpp
            messagewindow.ui
    )
endif ()

if (${CMAKE_SYSTEM_NAME}  MATCHES "Windows")
    target_sources(doublecontact
        PRIVATE
            doublecontact.rc
    )
endif ()
