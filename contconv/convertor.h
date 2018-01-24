/* Double Contact
 *
 * Module: console convertor application class
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <QCoreApplication>
#include <QString>
#include <QStringList>

#include "consoleasyncui.h"
#include "formats/iformat.h"
#include "formats/files/csvfile.h"

class Convertor : public QCoreApplication
{
    Q_OBJECT
public:
    Convertor(int &argc, char **argv);
    int start();
    void printUsage();
private:
    QTextStream out;
    ConsoleAsyncUI aui;
    void logFormat(IFormat* format);
    void setCSVProfile(CSVFile* csvFormat, const QString& code);
};

#endif // CONVERTOR_H
