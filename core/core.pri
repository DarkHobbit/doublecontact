# Core (GUI-independent) part of DoubleContact

QT += core xml

include(../3rdparty/quazip/quazip.pri)
include(../3rdparty/qwebdavlib/qwebdavlib/qwebdavlib.pri)

DEFINES += QUAZIP_STATIC
win32 {
  greaterThan(QT_MAJOR_VERSION, 4) {
LIBS += -lz
} else {
  INCLUDEPATH += C:/Qt/4.8.6.st/src/3rdparty/zlib
}
}
unix:LIBS += -lz

INCLUDEPATH += $$PWD

HEADERS	+= \
    $$PWD/contactlist.h \
    $$PWD/globals.h \
    $$PWD/languagemanager.h \
    $$PWD/formats/iformat.h \
    $$PWD/formats/formatfactory.h \
    $$PWD/formats/common/vcarddata.h \
    $$PWD/formats/files/csvfile.h \
    $$PWD/formats/files/fileformat.h \
    $$PWD/formats/files/mpbfile.h \
    $$PWD/formats/files/nbffile.h \
    $$PWD/formats/files/udxfile.h \
    $$PWD/formats/files/vcfdirectory.h \
    $$PWD/formats/files/vcffile.h \
    $$PWD/formats/network/asyncformat.h \
    $$PWD/formats/profiles/csvprofilebase.h \
    $$PWD/formats/profiles/explaybm50profile.h \
    $$PWD/formats/profiles/explaytv240profile.h \
    $$PWD/formats/profiles/genericcsvprofile.h \
    $$PWD/formats/profiles/osmoprofile.h \
    $$PWD/formats/files/htmlfile.h \
    ../core/formats/network/carddavformat.h

SOURCES	+= \
    $$PWD/contactlist.cpp \
    $$PWD/globals.cpp \
    $$PWD/languagemanager.cpp \
    $$PWD/formats/formatfactory.cpp \
    $$PWD/formats/common/vcarddata.cpp \
    $$PWD/formats/files/csvfile.cpp \
    $$PWD/formats/files/fileformat.cpp \
    $$PWD/formats/files/mpbfile.cpp \
    $$PWD/formats/files/nbffile.cpp \
    $$PWD/formats/files/udxfile.cpp \
    $$PWD/formats/files/vcfdirectory.cpp \
    $$PWD/formats/files/vcffile.cpp \
    $$PWD/formats/network/asyncformat.cpp \
    $$PWD/formats/profiles/csvprofilebase.cpp \
    $$PWD/formats/profiles/explaybm50profile.cpp \
    $$PWD/formats/profiles/explaytv240profile.cpp \
    $$PWD/formats/profiles/genericcsvprofile.cpp \
    $$PWD/formats/profiles/osmoprofile.cpp \
    $$PWD/formats/files/htmlfile.cpp \
    ../core/formats/network/carddavformat.cpp

