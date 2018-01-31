# Desktop application of DoubleContact

include(../core/core.pri)
include(../model/model.pri)

QT += gui
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets

TARGET = doublecontact
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    contactdialog.cpp \
    phonetypedialog.cpp \
    logwindow.cpp \
    settingsdialog.cpp \
    datedetailsdialog.cpp \
    helpers.cpp \
    comparecontainers.cpp \
    comparedialog.cpp \
    multicontactdialog.cpp \
    aboutdialog.cpp \
    languageselectdialog.cpp \
    csvprofiledialog.cpp \
    sortdialog.cpp \
    groupdialog.cpp

HEADERS += mainwindow.h \
    contactdialog.h \
    phonetypedialog.h \
    logwindow.h \
    settingsdialog.h \
    datedetailsdialog.h \
    helpers.h \
    comparecontainers.h \
    comparedialog.h \
    multicontactdialog.h \
    aboutdialog.h \
    languageselectdialog.h \
    csvprofiledialog.h \
    sortdialog.h \
    groupdialog.h

FORMS += mainwindow.ui \
    contactdialog.ui \
    phonetypedialog.ui \
    logwindow.ui \
    settingsdialog.ui \
    datedetailsdialog.ui \
    comparedialog.ui \
    multicontactdialog.ui \
    aboutdialog.ui \
    languageselectdialog.ui \
    csvprofiledialog.ui \
    sortdialog.ui \
    groupdialog.ui
unix { 
    OBJECTS_DIR = .obj
    UI_DIR = .ui
    MOC_DIR = .moc
}

TRANSLATIONS += \
    ../translations/doublecontact_de.ts \
    ../translations/doublecontact_en_GB.ts \
    ../translations/doublecontact_nb_NO.ts \
    ../translations/doublecontact_nl.ts \
    ../translations/doublecontact_ru_RU.ts \
    ../translations/doublecontact_uk_UA.ts

greaterThan(QT_MAJOR_VERSION, 4) {
win32 {
tr.commands = lrelease \
    $$_PRO_FILE_
}
unix&&!macx {
tr.commands = lrelease-qt5 \
    $$_PRO_FILE_
} else {
tr.commands = lrelease \
    $$_PRO_FILE_

}
} else {
tr.commands = lrelease \
    $$_PRO_FILE_
}
    
QMAKE_EXTRA_TARGETS += tr
POST_TARGETDEPS += tr

RESOURCES += \
    doublecontact.qrc
win32:RC_FILE = doublecontact.rc
macx:ICON = $${PWD}/../img/multi/doublecontact.icns
