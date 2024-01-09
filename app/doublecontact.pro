# Desktop application of DoubleContact

DEFINES += WITH_MESSAGES
DEFINES += WITH_CALLS

include(../core/core.pri)
include(../model/model.pri)

QT += gui
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets

TARGET = doublecontact
TEMPLATE = app
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    contactdialog.cpp \
    phonetypedialog.cpp \
    logwindow.cpp \
    settingsdialog.cpp \
    datedetailsdialog.cpp \
    helpers.cpp \
    comparecontainers.cpp \
    comparedialog.cpp \
    innerfilewindow.cpp \
    multicontactdialog.cpp \
    aboutdialog.cpp \
    languageselectdialog.cpp \
    csvprofiledialog.cpp \
    sortdialog.cpp \
    tagremovedialog.cpp \
    groupdialog.cpp

HEADERS += \
    mainwindow.h \
    contactdialog.h \
    phonetypedialog.h \
    logwindow.h \
    settingsdialog.h \
    datedetailsdialog.h \
    helpers.h \
    comparecontainers.h \
    comparedialog.h \
    innerfilewindow.h \
    multicontactdialog.h \
    aboutdialog.h \
    languageselectdialog.h \
    csvprofiledialog.h \
    sortdialog.h \
    tagremovedialog.h \
    groupdialog.h    

FORMS += \
    mainwindow.ui \
    contactdialog.ui \
    phonetypedialog.ui \
    logwindow.ui \
    settingsdialog.ui \
    datedetailsdialog.ui \
    comparedialog.ui \
    innerfilewindow.ui \
    multicontactdialog.ui \
    aboutdialog.ui \
    languageselectdialog.ui \
    csvprofiledialog.ui \
    sortdialog.ui \
    tagremovedialog.ui \
    groupdialog.ui

contains(DEFINES, WITH_MESSAGES) {
SOURCES += messagewindow.cpp
HEADERS += messagewindow.h
FORMS += messagewindow.ui
}

contains(DEFINES, WITH_CALLS) {
SOURCES += callwindow.cpp
HEADERS += callwindow.h
FORMS += callwindow.ui
}

unix { 
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}

TRANSLATIONS += \
    ../translations/doublecontact_de.ts \
    ../translations/doublecontact_en_GB.ts \
    ../translations/doublecontact_fr.ts \
    ../translations/doublecontact_ie.ts \
    ../translations/doublecontact_nb_NO.ts \
    ../translations/doublecontact_nl.ts \
    ../translations/doublecontact_pl.ts \
    ../translations/doublecontact_pt.ts \
    ../translations/doublecontact_pt_BR.ts \
    ../translations/doublecontact_ru_RU.ts \
    ../translations/doublecontact_uk_UA.ts \
    ../translations/doublecontact_zh_Hant.ts



exists( $$dirname(QMAKE_QMAKE)/lrelease-qt4 ) {
      tr.commands = $$dirname(QMAKE_QMAKE)/lrelease-qt4  $$_PRO_FILE_
} else:exists( $$dirname(QMAKE_QMAKE)/lrelease-qt5 ) {
      tr.commands = $$dirname(QMAKE_QMAKE)/lrelease-qt5  $$_PRO_FILE_
} else {
      tr.commands = $$dirname(QMAKE_QMAKE)/lrelease  $$_PRO_FILE_
}
    
QMAKE_EXTRA_TARGETS += tr
POST_TARGETDEPS += tr

RESOURCES += \
    doublecontact.qrc
win32:RC_FILE = doublecontact.rc
macx:ICON = $${PWD}/../img/multi/doublecontact.icns
