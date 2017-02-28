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
#include "csvprofilebase.h"

QString CSVProfileBase::name() const
{
    return _name;
}

bool CSVProfileBase::hasHeader() const
{
    return _hasHeader;
}

QString CSVProfileBase::charSet() const
{
    return _charSet;
}

bool CSVProfileBase::hasBOM() const
{
    return _hasBOM;
}

CSVProfileBase::QuotingPolicy CSVProfileBase::quotingPolicy() const
{
    return _quotingPolicy;
}

bool CSVProfileBase::parseHeader(const QStringList&)
{
    return true;
}

bool CSVProfileBase::prepareExport(const ContactList&)
{
    return true;
}

QStringList CSVProfileBase::makeHeader()
{
    return QStringList();
}

bool CSVProfileBase::condAddPhone(const QStringList &row, ContactItem &item, int index, const QString &phType)
{
    if (row.count()>index && !row[index].isEmpty()) {
        item.phones << Phone(row[index], phType);
        return true;
    }
    else
        return false;
}

bool CSVProfileBase::condReadValue(const QStringList &row, int index, QString &dest)
{
    if (row.count()>index && !row[index].isEmpty()) {
        dest = row[index];
        return true;
    }
    else
        return false;
}

bool CSVProfileBase::readWarning(const QStringList &row, int index, QStringList& errors)
{
    if (row.count()>index && !row[index].isEmpty()) {
        errors << QString("Column %1 is not empty (%2). Please, contact author").arg(index).arg(row[index]); // Don't translate!
        return true;
    }
    else
        return false;
}

QString CSVProfileBase::saveNamePart(const ContactItem &item, int nameIndex)
{
    if (item.names.count()>nameIndex && !item.names[nameIndex].isEmpty())
        return item.names[nameIndex];
    else
        return "";
}

