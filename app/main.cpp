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

#include "configmanager.h"
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
    if (!languageManager.load(LanguageManager::transPath()+QDir::separator()+QString("iso639-1.utf8")))
        QMessageBox::warning(0, S_WARNING, "Language list loading error");
    else {
        if (language.isEmpty()) {
            language = LanguageSelectDialog::selectLanguage();
            configManager.writeLanguage(language);
        }
        QString langCode = languageManager.nativeNameToCode(language);
        QString langPath = LanguageManager::transPath()+QDir::separator()+QString("doublecontact_%1.qm").arg(langCode);
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
    // Arguments parse TODO m.b. move this code to separate file for QML support
    gd.debugDataMode = false;
    gd.fullScreenMode = false;
    gd.quietMode = false;
    gd.startupFiles.clear();
    for (int i=1; i<qApp->arguments().count(); i++) { // foreach not work, because arg0 is program name
        const QString ar = qApp->arguments()[i];
        if (ar=="--debugdata" ||ar=="-d")
            gd.debugDataMode = true;
        else if (ar=="--quiet" || ar=="-q")
            gd.quietMode = true;
        else if (ar=="--fullscreen" || ar=="-f")
            gd.fullScreenMode = true;
        else if (gd.startupFiles.count()<2)
            gd.startupFiles << ar;
    }
    // Main Window
    MainWindow w;
    if (gd.fullScreenMode)
        w.showMaximized();
    else
        w.show();
    return a.exec();
}
