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

#include "mainwindow.h"
#include <QApplication>

#include <QDir>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Settings settings :)
    a.setOrganizationName("doublecontact");
    a.setApplicationName("doublecontact");
    // TODO temporary language stub
    QTranslator tr;
    tr.load(qApp->applicationDirPath()+QDir::separator()+"doublecontact_ru_RU.qm");
        qApp->installTranslator(&tr);
    // TODO end of stub
    MainWindow w;
    if (qApp->arguments().contains("--fullscreen"))
        w.showMaximized();
    else
        w.show();
    return a.exec();
}
