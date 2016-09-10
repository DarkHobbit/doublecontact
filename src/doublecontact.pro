# -------------------------------------------------
# Project created by QtCreator 2015-07-24T00:39:39
# -------------------------------------------------
QT += core \
    gui \
    xml
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
TARGET = doublecontact
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    contactmodel.cpp \
    contactlist.cpp \
    contactdialog.cpp \
    phonetypedialog.cpp \
    formats/formatfactory.cpp \
    formats/files/fileformat.cpp \
    formats/files/vcffile.cpp \
    formats/common/vcarddata.cpp \
    logwindow.cpp \
    settingsdialog.cpp \
    globals.cpp \
    datedetailsdialog.cpp \
    contactsorterfilter.cpp \
    formats/files/udxfile.cpp \
    comparecontainers.cpp \
    comparedialog.cpp \
    multicontactdialog.cpp \
    formats/files/vcfdirectory.cpp \
    formats/files/mpbfile.cpp \
    aboutdialog.cpp
HEADERS += mainwindow.h \
    contactmodel.h \
    contactlist.h \
    contactdialog.h \
    phonetypedialog.h \
    formats/iformat.h \
    formats/formatfactory.h \
    formats/files/fileformat.h \
    formats/files/vcffile.h \
    formats/common/vcarddata.h \
    logwindow.h \
    settingsdialog.h \
    globals.h \
    datedetailsdialog.h \
    contactsorterfilter.h \
    formats/files/udxfile.h \
    comparecontainers.h \
    comparedialog.h \
    multicontactdialog.h \
    formats/files/vcfdirectory.h \
    formats/files/mpbfile.h \
    aboutdialog.h
FORMS += mainwindow.ui \
    contactdialog.ui \
    phonetypedialog.ui \
    logwindow.ui \
    settingsdialog.ui \
    datedetailsdialog.ui \
    comparedialog.ui \
    multicontactdialog.ui \
    aboutdialog.ui
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
