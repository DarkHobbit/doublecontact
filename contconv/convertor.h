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
#include <QTextStream>
#include "formats/iformat.h"
#include "formats/files/csvfile.h"
#include "formats/network/asyncformat.h"

class Convertor : public QCoreApplication, IAsyncUI
{
    Q_OBJECT
public:
    Convertor(int &argc, char **argv);
    int start();
    void printUsage();
    // IAsyncUI implementation
    virtual QString inputPassword();
    virtual bool securityConfirm(const QString& question);
private:
    QTextStream out, in;
    void logFormat(IFormat* format);
    void setCSVProfile(CSVFile* csvFormat, const QString& code);
};

#endif // CONVERTOR_H
