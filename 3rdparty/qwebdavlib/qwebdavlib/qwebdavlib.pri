QT       += network xml
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += QWEBDAV_LIBRARY

SOURCES += \
    $$PWD/qwebdav.cpp \
    $$PWD/qwebdavitem.cpp \
    $$PWD/qwebdavdirparser.cpp \
    $$PWD/qnaturalsort.cpp

HEADERS += \
    $$PWD/qwebdav.h \
    $$PWD/qwebdavitem.h \
    $$PWD/qwebdavdirparser.h \
    $$PWD/qnaturalsort.h \
    $$PWD/qwebdav_global.h

