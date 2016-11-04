# Model-related part of DoubleContact

include(../core/core.pri)
lessThan(QT_MAJOR_VERSION, 5):QT += gui

INCLUDEPATH += $$PWD

HEADERS	+= \
    $$PWD/contactmodel.h \
    $$PWD/contactsorterfilter.h \
    $$PWD/recentlist.h \
    $$PWD/configmanager.h

SOURCES	+= \
    $$PWD/contactmodel.cpp \
    $$PWD/contactsorterfilter.cpp \
    $$PWD/recentlist.cpp \
    $$PWD/configmanager.cpp

