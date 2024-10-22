# Core (GUI-independent) part of DoubleContact

# To add network support, including CardDAV, define WITH_NETWORK name
# To use code under GPLv2, switch on USE_GPL2 define.
# NBU support will be disabled
DEFINES -= USE_GPL2

QT += core xml

include(../3rdparty/quazip/quazip.pri)

DEFINES += QUAZIP_STATIC

contains(CONFIG, static){
  INCLUDEPATH += $(QTDIR)/src/3rdparty/zlib
}
win32 {
  INCLUDEPATH += $(QTDIR)/src/3rdparty/zlib
  greaterThan(QT_MAJOR_VERSION, 4) {
LIBS += -lz
}
}
unix:LIBS += -lz


INCLUDEPATH += $$PWD

HEADERS	+= \
    $$PWD/bstring.h \
    $$PWD/contactlist.h \
    $$PWD/corehelpers.h \
    $$PWD/extra.h \
    $$PWD/globals.h \
    $$PWD/languagemanager.h \
    $$PWD/formats/iformat.h \
    $$PWD/formats/formatfactory.h \
    $$PWD/formats/common/quotedprintable.h \
    $$PWD/formats/common/textreport.h \
    $$PWD/formats/common/vcarddata.h \
    $$PWD/formats/common/vdata.h \
    $$PWD/formats/files/csvfile.h \
    $$PWD/formats/files/fileformat.h \
    $$PWD/formats/files/htmlfile.h \
    $$PWD/formats/files/mpbfile.h \
    $$PWD/formats/files/nbffile.h \
    $$PWD/formats/files/udxfile.h \
    $$PWD/formats/files/vcfdirectory.h \
    $$PWD/formats/files/vcffile.h \
    $$PWD/formats/files/xmlcontactfile.h \
    $$PWD/formats/network/asyncformat.h \
    $$PWD/formats/profiles/csvprofilebase.h \
    $$PWD/formats/profiles/explaybm50profile.h \
    $$PWD/formats/profiles/explaytv240profile.h \
    $$PWD/formats/profiles/genericcsvprofile.h \
    $$PWD/formats/profiles/osmoprofile.h \
    $$PWD/formats/profiles/sylpheedprofile.h \

SOURCES	+= \
    $$PWD/bstring.cpp \
    $$PWD/contactlist.cpp \
    $$PWD/corehelpers.cpp \
    $$PWD/extra.cpp \
    $$PWD/globals.cpp \
    $$PWD/languagemanager.cpp \
    $$PWD/formats/formatfactory.cpp \
    $$PWD/formats/common/quotedprintable.cpp \
    $$PWD/formats/common/textreport.cpp \
    $$PWD/formats/common/vcarddata.cpp \
    $$PWD/formats/common/vdata.cpp \
    $$PWD/formats/files/csvfile.cpp \
    $$PWD/formats/files/fileformat.cpp \
    $$PWD/formats/files/htmlfile.cpp \
    $$PWD/formats/files/mpbfile.cpp \
    $$PWD/formats/files/nbffile.cpp \
    $$PWD/formats/files/udxfile.cpp \
    $$PWD/formats/files/vcfdirectory.cpp \
    $$PWD/formats/files/vcffile.cpp \
    $$PWD/formats/files/xmlcontactfile.cpp \
    $$PWD/formats/network/asyncformat.cpp \
    $$PWD/formats/profiles/csvprofilebase.cpp \
    $$PWD/formats/profiles/explaybm50profile.cpp \
    $$PWD/formats/profiles/explaytv240profile.cpp \
    $$PWD/formats/profiles/genericcsvprofile.cpp \
    $$PWD/formats/profiles/osmoprofile.cpp \
    $$PWD/formats/profiles/sylpheedprofile.cpp \

contains(DEFINES, WITH_MESSAGES) {
    HEADERS += \
    $$PWD/decodedmessagelist.h \
    $$PWD/formats/common/nokiadata.h \
    $$PWD/formats/common/pdu.h \
    $$PWD/formats/common/vmessagedata.h
    SOURCES += \
    $$PWD/decodedmessagelist.cpp \
    $$PWD/formats/common/nokiadata.cpp \
    $$PWD/formats/common/pdu.cpp \
    $$PWD/formats/common/vmessagedata.cpp
}

contains(DEFINES, WITH_NETWORK) {
    include(../3rdparty/qwebdavlib/qwebdavlib/qwebdavlib.pri)
    INCLUDEPATH += ../3rdparty/qwebdavlib/qwebdavlib
    HEADERS += \
        $$PWD/formats/network/carddavformat.h \
        $$PWD/formats/network/imageloader.h
    SOURCES += \
        $$PWD/formats/network/carddavformat.cpp \
        $$PWD/formats/network/imageloader.cpp
}

# TODO move here pdu and nokiadata
!contains(DEFINES, USE_GPL2) {
    HEADERS	+= $$PWD/formats/files/nbufile.h
    SOURCES	+= $$PWD/formats/files/nbufile.cpp
}
