            #-------------------------------------------------
    #
    # Project created by QtCreator 2016-10-30T20:09:54
    #
    #-------------------------------------------------

    QT       += core
    QT       -= gui
    DEFINES -= WITH_NETWORK
    include(../core/core.pri)

    TARGET = contconv
    CONFIG   += console
    CONFIG   -= app_bundle


    TEMPLATE = app


    SOURCES += main.cpp \
        convertor.cpp \
        consoleasyncui.cpp

    HEADERS += \
        convertor.h \
        consoleasyncui.h

unix { 
    OBJECTS_DIR = .obj
    MOC_DIR = .moc
}
