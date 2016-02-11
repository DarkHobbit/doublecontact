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

class IFormat {
public:
    virtual ~IFormat();
    virtual bool detect(const QString& url)=0; // tri-state?
    virtual QStringList supportedTypes()=0; // only for files
    virtual bool importRecords(const QString& url, ContactList& list, bool append)=0;
    virtual bool exportRecords(const QString& url, const ContactList& list)=0;
};

#endif // IFORMAT_H
