/* Double Contact
 *
 * Module: Abstract class for CSV file profiles
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef CSVPROFILEBASE_H
#define CSVPROFILEBASE_H

#include <QString>
#include <QStringList>
#include "../../contactlist.h"

#define S_CSV_ROW_TOO_SHORT QObject::tr("CSV row too short for this profile at line %1")

class CSVProfileBase
{
public:
    virtual ~CSVProfileBase() {};
    virtual bool detect(const QStringList& header) const=0;
    virtual bool hasHeader() const=0;
    // Read
    virtual bool parseHeader(const QStringList& header);
    virtual bool importRecord(const QStringList& row, ContactItem& item, QStringList& errors)=0;
    // Write
    virtual bool prepareExport(const ContactList &list);
    virtual QStringList makeHeader();
    virtual bool exportRecord(QStringList& row, const ContactItem& item, QStringList& errors)=0;
protected:
    bool condAddPhone(const QStringList &row, ContactItem &item, int index, const QString& phType);
    bool condReadValue(const QStringList &row, int index, QString& dest);
    bool readWarning(const QStringList &row, int index, QStringList& errors);
    QString saveNamePart(const ContactItem &item, int nameIndex);
};

#endif // CSVPROFILEBASE_H
