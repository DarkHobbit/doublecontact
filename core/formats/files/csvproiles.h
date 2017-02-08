/* Double Contact
 *
 * Module: Various CSV file profiles
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef CSVPROILES_H
#define CSVPROILES_H

#include <QString>
#include <QStringList>
#include "../../contactlist.h"


class CSVProileBase
{
public:
    virtual ~CSVProileBase() {};
    virtual bool detect(const QStringList& header) const=0;
    virtual bool hasHeader() const=0;
    // Read
    virtual bool parseHeader(const QStringList& header);
    virtual bool importRecord(const QStringList& row, ContactItem& item, QStringList& errors)=0;
    // Write
    virtual bool prepareExport(const ContactList &list);
    virtual bool exportRecord(QStringList& row, const ContactItem& item)=0;
protected:
    bool condAddPhone(const QStringList &row, ContactItem &item, int index, const QString& phType);
};

class ExplayCSVProfile: public CSVProileBase
{
public:
    ExplayCSVProfile();
    virtual bool detect(const QStringList& header) const;
    virtual bool hasHeader() const;
    // Read
    virtual bool importRecord(const QStringList& row, ContactItem& item, QStringList& errors);
    // Write
    virtual bool exportRecord(QStringList& row, const ContactItem& item);
};

#endif // CSVPROILES_H
