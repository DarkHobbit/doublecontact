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

#include "../gui/configmanager.h"
#include "languagemanager.h"
#include "languageselectdialog.h"
#include "mainwindow.h"
#include "settingsdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Set UI language
    QString language = configManager.readLanguage();
    QTranslator tr;
    if (!languageManager.load(a.applicationDirPath()+QDir::separator()+QString("iso639-1.utf8")))
        QMessageBox::warning(0, S_WARNING, "Language list loading error");
    else {
        if (language.isEmpty()) {
            language = LanguageSelectDialog::selectLanguage();
            configManager.writeLanguage(language);
        }
        QString langCode = languageManager.nativeNameToCode(language);
        QString langPath = a.applicationDirPath()+QDir::separator()+QString("doublecontact_%1.qm").arg(langCode);
        if (!tr.load(langPath))
            QMessageBox::warning(0, S_WARNING, "UI loading error");
        else {
            a.installTranslator(&tr);
            Phone::standardTypes.fill();
            Email::standardTypes.fill();
            contactColumnHeaders.fill();
        }
    }
    configManager.readConfig(); // After contactColumnHeaders.fill()! Else national UI not works
    // Main Window
    MainWindow w;
    if (qApp->arguments().contains("--fullscreen") || qApp->arguments().contains("-f"))
        w.showMaximized();
    else
        w.show();
    return a.exec();
}
