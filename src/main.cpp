/* Double Contact
 *
 * Module: Main
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>

#include "mainwindow.h"
#include "settingsdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Settings settings :)
    a.setOrganizationName("doublecontact");
    a.setApplicationName("doublecontact");
    // Set UI language
    QSettings settings;
    QString language = SettingsDialog::readLanguage(settings);
    QString langCode = "en_GB";
    if (language==QString::fromUtf8("Русский")) // TODO dyn load?
        langCode = "ru_RU";
    QString langPath = a.applicationDirPath()+QDir::separator()+QString("doublecontact_%1.qm").arg(langCode);
    QTranslator tr;
    if (!tr.load(langPath))
        QMessageBox::critical(0, S_ERROR, "UI loading error");
    else {
        a.installTranslator(&tr);
        Phone::standardTypes.fill();
        Email::standardTypes.fill();
        contactColumnHeaders.fill();
    }
    // Main Window
    MainWindow w;
    if (qApp->arguments().contains("--fullscreen") || qApp->arguments().contains("-f"))
        w.showMaximized();
    else
        w.show();
    return a.exec();
}
