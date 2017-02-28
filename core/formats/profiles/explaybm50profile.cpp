/* Double Contact
 *
 * Module: CSV file profile for Explay BM50
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "explaybm50profile.h"
#include <iostream>

ExplayBM50Profile::ExplayBM50Profile()
{
    _name = "Explay BM50";
    _hasHeader = true;
    _charSet = "UTF-16LE";
    _hasBOM = true;
    _quotingPolicy = CSVProfileBase::AlwaysQuote;
    _lineEnding = CSVProfileBase::CRLFEnding;
}

bool ExplayBM50Profile::detect(const QStringList &header) const
{
    return
        header[0]=="Title"
        && header[1]=="First name"
            && header[2]=="Middle name";
}

bool ExplayBM50Profile::importRecord(const QStringList &row, ContactItem &item, QStringList& errors)
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
    if (!row[7].isEmpty()) {
        item.birthday.value = QDateTime::fromString(row[7]); // TODO check
        errors << QString("Explay birthday in %1 format (%2). Please, contact author")
            .arg(row[7]).arg(item.fullName); // Don't translate!
    }
    readWarning(row, 8, errors); // TODO SIP address
    readWarning(row, 9, errors); // TODO Push-to-talk
    readWarning(row, 10, errors); // TODO Share view
    item.id = row[11];
    condAddPhone(row, item, 12, "pref");
    condAddPhone(row, item, 13, "home");
    condAddPhone(row, item, 14, "fax");
    condAddPhone(row, item, 15, "video");
    condReadValue(row, 16, item.url);
    // TODO see other rows and check on real phone
    return true;
}

QStringList ExplayBM50Profile::makeHeader()
{
    return QStringList()
        << "Title" << "First name" << "Middle name" << "Last name" << "Suffix" //[0-4]
        << "Job title" << "Company" << "Birthday" // [5-7]
        << "SIP address" << "Push-to-talk" << "Share view" << "User ID" // [8-11]
        << "General mobile" << "General phone" << "General fax" << "General video call" //[12-15]
        << "General web address" << "General VOIP address" //[16-17]
        << "General P.O.Box" << "General extension" << "General street" << "General postal/ZIP code" << "General city" << "General state/province" << "General country/region" //[18-24]
        << "Home mobile" << "Home phone" << "Home email" << "Home fax" << "Home video call" //[25-29]
        << "Home web address" << "Home VOIP address" //[30-31]
        << "Home P.O.Box" << "Home extension" << "Home street" << "Home postal/ZIP code" << "Home city" << "Home state/province" << "Home country/region" //[32-38]
        << "Business mobile" << "Business phone" << "Business email" << "Business fax" << "Business video call" //[39-43]
        << "Business web address" << "Business VOIP address" //[[44-45]
        << "Business P.O.Box" << "Business extension" << "Business street" << "Business postal/ZIP code" << "Business city" << "Business state/province" << "Business country/retion"//[46-52]
        << ""; //[53]
}

bool ExplayBM50Profile::exportRecord(QStringList &row, const ContactItem &item, QStringList& errors)
{
    row << item.fullName; //[0]
    row << saveNamePart(item, 1);
    row << saveNamePart(item, 2);
    row << saveNamePart(item, 0); //[3]
    row << saveNamePart(item, 4);
    if (item.names.count()>3 && !item.names[3].isEmpty()) {
        row[3] = item.names[3] + " " + row[3];
        errors << QObject::tr("Name prefixes merged with last name (%1)").arg(row[3]);
    }
    row << item.title;
    row << item.organization; // [6]
    if (!item.birthday.isEmpty()) {
        row << item.birthday.toString(DateItem::ISOBasic); // TODO check
        errors << QString("Explay birthday in %1 format (%2). Please, contact author")
            .arg(row.last()).arg(item.fullName); // Don't translate!
    }
    else
        row << ""; // [7]
    row << "" << "" << ""; // TODO SIP address, Push-to-talk, Share view
    row << item.id; // [11]
    // One phone
    row << item.prefPhone; // [12]
    // TODO see other rows and check on real phone
    for (int i=13; i<54; i++)
        row << "";
    // TODO write warnings on non-empty unsupported fields, sync with udxfile
    return true;
}

