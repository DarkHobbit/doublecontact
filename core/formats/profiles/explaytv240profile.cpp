/* Double Contact
 *
 * Module: CSV file profile for Explay TV240
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "explaytv240profile.h"
#include <iostream>

ExplayTV240Profile::ExplayTV240Profile()
{
    _name = "Explay TV240";
    _hasHeader = true;
    _charSet = "UTF-16LE";
    _hasBOM = false; //TODO check on 4PDA
    _quotingPolicy = CSVProfileBase::NeverQuote; // TODO check on 4PDA
}

bool ExplayTV240Profile::detect(const QStringList &header) const
{
    return
        header[0]=="Name"
        && header[1]=="Number"
            && header[2]=="Home Number";
}

bool ExplayTV240Profile::importRecord(const QStringList &row, ContactItem &item, QStringList& errors)
{
    if (row.count()<2) {
        errors << S_CSV_ROW_TOO_SHORT.arg(row.join(","));
        return false;
    }
    item.names << row[0];
    item.phones << Phone(row[1], "PREF");
    return true;
}

QStringList ExplayTV240Profile::makeHeader()
{
    return QStringList()
        << "Name" << "Number" << "Home Number" << "Company Name"
        << "E-mail Address" << "Office Number" << "Fax Number"; //[6]
}

bool ExplayTV240Profile::exportRecord(QStringList &row, const ContactItem &item, QStringList& errors)
{
    // TODO don't quote!!!
    row << saveNamePart(item, 0);
    // One phone
    row << item.prefPhone;
    for (int i=2; i<7; i++)
        row << "";
    // TODO write warnings on non-empty unsupported fields, sync with udxfile
    return true;
}

