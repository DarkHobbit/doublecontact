/* Double Contact
 *
 * Module: CSV file profile for full vCard data saving
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "genericcsvprofile.h"
#include <iostream>

GenericCSVProfile::GenericCSVProfile()
{
    clearCounters();
    _name = QObject::tr("Generic profile");
    _hasHeader = true;
    _charSet = "UTF-8"; // TODO select encoding in settings
    _hasBOM = false;
    _quotingPolicy = CSVProfileBase::AlwaysQuote;
    _lineEnding = CSVProfileBase::CRLFEnding;
}

bool GenericCSVProfile::detect(const QStringList &header) const
{
// TODO
}

bool GenericCSVProfile::parseHeader(const QStringList &header)
{
    _header = header;
    return (!header.isEmpty());
}

bool GenericCSVProfile::importRecord(const QStringList &row, ContactItem &item, QStringList& errors)
{
    // TODO - check if row length != header length
}

bool GenericCSVProfile::prepareExport(const ContactList &list)
{
    if (list.isEmpty())
        return false;
    clearCounters();
    foreach (const ContactItem &item, list) {
        if (!item.fullName.isEmpty())
            hasFullNames = true;
        if (!item.names.isEmpty())
            hasNames = true;
        if (!item.phones.count()>phoneCount)
            phoneCount = item.phones.count(); // No! various types!!!

    }

    return true;
}

QStringList GenericCSVProfile::makeHeader()
{
    // TODO
}

bool GenericCSVProfile::exportRecord(QStringList &row, const ContactItem &item, QStringList& errors)
{
    // TODO
}

void GenericCSVProfile::clearCounters()
{
    _header.clear();
    hasFullNames = false;
    hasNames = false;
    phoneCount = 0;
    emailCount = 0;
    hasBDay = false;
    hasAnn = false;
    hasSortString = false;
    // ...
    addrCount = 0;
    // ...
}

