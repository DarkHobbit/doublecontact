/* Double Contact
 *
 * Module: Abstract interface for file/network export/import format
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef IFORMAT_H
#define IFORMAT_H

#include <QString>
#include <QStringList>
#include "../contactlist.h"

enum FormatType {
    ftNew,
    ftFile,
    ftDirectory,
#ifdef WITH_NETWORK
    ftNetwork,
#endif
    ftAuto // file or dir - only for open existing source
};

class IFormat {
public:
    virtual ~IFormat() {};
    virtual bool importRecords(const QString& url, ContactList& list, bool append)=0;
    virtual bool exportRecords(const QString& url, ContactList& list)=0;
    virtual QStringList errors()=0;
    virtual QString fatalError()=0;
    /* Subclasses also can create next _static_ methods:
     * bool detect(const QString& url); // tri-state?
     * QStringList supportedExtensions(); // only for files
     * QStringList supportedFilters();    // only for files
     */
};

#endif // IFORMAT_H
