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
    formats/formatfactory.cpp
HEADERS += mainwindow.h \
    contactmodel.h \
    contactlist.h \
    contactdialog.h \
    phonetypedialog.h \
    formats/iformat.h \
    formats/formatfactory.h
FORMS += mainwindow.ui \
    contactdialog.ui \
    phonetypedialog.ui
unix { 
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}

RESOURCES += \
    doublecontact.qrc
