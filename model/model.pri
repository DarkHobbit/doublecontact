# Model-related part of DoubleContact

QT += gui

INCLUDEPATH += $$PWD

HEADERS	+= \
    $$PWD/contactmodel.h \
    $$PWD/modelhelpers.h \
    $$PWD/recentlist.h \
    $$PWD/configmanager.h

SOURCES	+= \
    $$PWD/contactmodel.cpp \
    $$PWD/modelhelpers.cpp \
    $$PWD/recentlist.cpp \
    $$PWD/configmanager.cpp

contains(DEFINES, WITH_MESSAGES) {
HEADERS	+= $$PWD/messagemodel.h
SOURCES	+= $$PWD/messagemodel.cpp
}

contains(DEFINES, WITH_MESSAGES) {
HEADERS	+= $$PWD/callmodel.h
SOURCES	+= $$PWD/callmodel.cpp
}
