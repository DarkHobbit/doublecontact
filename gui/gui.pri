# GUI-dependent (but QtWidgets-independent) part of DoubleContact

include(../core/core.pri)

QT += gui

INCLUDEPATH += $$PWD

HEADERS	+= \
    $$PWD/recentlist.h \
    $$PWD/configmanager.h

SOURCES	+= \
    $$PWD/recentlist.cpp \
    $$PWD/configmanager.cpp

