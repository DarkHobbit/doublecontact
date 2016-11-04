# Desktop application of DoubleContact

include(../model/model.pri)

QT += gui
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets

TARGET = doublecontact
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    contactdialog.cpp \
    phonetypedialog.cpp \
    logwindow.cpp \
    settingsdialog.cpp \
    datedetailsdialog.cpp \
    helpers.cpp \
    comparecontainers.cpp \
    comparedialog.cpp \
    multicontactdialog.cpp \
    aboutdialog.cpp \
    languageselectdialog.cpp

HEADERS += mainwindow.h \
    contactdialog.h \
    phonetypedialog.h \
    logwindow.h \
    settingsdialog.h \
    datedetailsdialog.h \
    helpers.h \
    comparecontainers.h \
    comparedialog.h \
    multicontactdialog.h \
    aboutdialog.h \
    languageselectdialog.h

FORMS += mainwindow.ui \
    contactdialog.ui \
    phonetypedialog.ui \
    logwindow.ui \
    settingsdialog.ui \
    datedetailsdialog.ui \
    comparedialog.ui \
    multicontactdialog.ui \
    aboutdialog.ui \
    languageselectdialog.ui
unix { 
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}

TRANSLATIONS += \
    ../translations/doublecontact_en_GB.ts \
    ../translations/doublecontact_ru_RU.ts

tr.commands = lrelease \
    $$_PRO_FILE_
QMAKE_EXTRA_TARGETS += tr
POST_TARGETDEPS += tr
RESOURCES += \
    doublecontact.qrc
