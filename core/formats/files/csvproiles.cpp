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
#include <iostream>

#define S_CSV_ROW_TOO_SHORT QObject::tr("CSV row too short for this profile at line %1")

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

QStringList ExplayCSVProfile::makeHeader()
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

bool ExplayCSVProfile::exportRecord(QStringList &row, const ContactItem &item, QStringList& errors)
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
        row << "";
    row << "" << "" << ""; // TODO SIP address, Push-to-talk, Share view
    row << item.id; // [11]
    // One phone
    row << item.prefPhone;
    // TODO see other rows and check on real phone
    for (int i=12; i<54; i++)
        row << "";
    // TODO write warnings on non-empty unsupported fields, sync with udxfile
    return true;
}

