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
#include "csvproiles.h"

#define S_CSV_ROW_TOO_SHORT QObject::tr("CSV row too short for this profile at line %1")

bool CSVProileBase::parseHeader(const QStringList&)
{
    return true;
}

bool CSVProileBase::prepareExport(const ContactList&)
{
    return true;
}

bool CSVProileBase::condAddPhone(const QStringList &row, ContactItem &item, int index, const QString &phType)
{
    if (row.count()>index && !row[index].isEmpty()) {
        item.phones << Phone(row[index], phType);
        return true;
    }
    else
        return false;
}

ExplayCSVProfile::ExplayCSVProfile()
{}

bool ExplayCSVProfile::detect(const QStringList &header) const
{
    return
        header[0]=="Title"
        && header[1]=="First name"
            && header[2]=="Middle name";
}

bool ExplayCSVProfile::hasHeader() const
{
    return true;
}

bool ExplayCSVProfile::importRecord(const QStringList &row, ContactItem &item, QStringList& errors)
{
    // TODO not all fields
    if (row.count()<13) {
        errors << S_CSV_ROW_TOO_SHORT.arg(row.join(","));
        return false;
    }
    item.fullName = row[0];
    item.names << row[3] << row[1] << row[2] << "" << row[4];
    item.dropFinalEmptyNames();
    item.title = row[5];
    item.organization = row[6];
    item.birthday.value = QDateTime::fromString(row[7]); // TODO check
    // "SIP address","Push-to-talk","Share view"
    item.id = row[11];
    condAddPhone(row, item, 12, "pref");
    condAddPhone(row, item, 13, "home");
    condAddPhone(row, item, 14, "fax");
    condAddPhone(row, item, 15, "video");
    if (row.count()>16)
        item.url = row[16];
    // TODO see other rows...
    return true;
}

bool ExplayCSVProfile::exportRecord(QStringList &row, const ContactItem &item)
{
    // TODO
    return false;
}

