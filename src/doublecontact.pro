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
    formats/files/udxfile.cpp
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
    formats/files/udxfile.h
FORMS += mainwindow.ui \
    contactdialog.ui \
    phonetypedialog.ui \
    logwindow.ui \
    settingsdialog.ui \
    datedetailsdialog.ui
unix { 
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}

RESOURCES += \
    doublecontact.qrc
