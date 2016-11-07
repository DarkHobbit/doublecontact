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

class Convertor : public QCoreApplication
{
public:
    Convertor(int &argc, char **argv);
    int start();
    void printUsage();
private:
    QTextStream out;
    void logFormat(IFormat* format);
};

#endif // CONVERTOR_H
