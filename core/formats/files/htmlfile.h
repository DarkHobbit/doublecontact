/* Double Contact
 *
 * Module: HTML file report
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef HTMLFILE_H
#define HTMLFILE_H

#include <QList>
#include <QTextStream>
#include "fileformat.h"

class HTMLFile : public FileFormat
{
public:
    HTMLFile();

    // IFormat interface
public:
    static QStringList supportedExtensions();
    static QStringList supportedFilters();
    virtual bool importRecords(const QString &, ContactList &, bool);
    virtual bool exportRecords(const QString &url, ContactList &list);
};

#endif // HTMLFILE_H
