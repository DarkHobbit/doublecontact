/* Double Contact
 *
 * Module: Data and container structures
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "contactlist.h"
#include <QMessageBox>

Phone::StandardTypes::StandardTypes()
{
    clear();
    // Types according RFC 2426
    (*this)["home"] = QObject::tr("Home");
    (*this)["msg"] = QObject::tr("Message");
    (*this)["work"] = QObject::tr("Work");
    (*this)["pref"] = QObject::tr("Preferable");
    (*this)["voice"] = QObject::tr("Voice"); // Synonym for OTHER for some real phones
    (*this)["fax"] = QObject::tr("Fax");
    (*this)["cell"] = QObject::tr("Cell");
    (*this)["video"] = QObject::tr("Video");
    (*this)["pager"] = QObject::tr("Pager");
    (*this)["bbs"] = QObject::tr("BBS");
    (*this)["modem"] = QObject::tr("Modem");
    (*this)["car"] = QObject::tr("Car");
    (*this)["isdn"] = QObject::tr("ISDN");
    (*this)["pcs"] = QObject::tr("PCS");
    displayValues
        << (*this)["home"]  << (*this)["work"] << (*this)["cell"]
        << (*this)["pref"] << (*this)["voice"]
        << (*this)["msg"] << (*this)["fax"] << (*this)["video"]
        << (*this)["pager"] << (*this)["bbs"]
        << (*this)["modem"] << (*this)["car"]
        << (*this)["isdn"] << (*this)["pcs"];
}

Email::StandardTypes::StandardTypes()
{
    clear();
    // Types according RFC 2426
    (*this)["internet"] = QObject::tr("Internet");
    (*this)["x400"] = QObject::tr("X.400");
    (*this)["pref"] = QObject::tr("Preferable");
    displayValues
        << (*this)["internet"]  << (*this)["x400"] << (*this)["pref"];
}

void ContactItem::clear()
{
    fullName.clear();
    names.clear();
    phones.clear();
    emails.clear();
    birthday.value = QDateTime();
    birthday.hasTime = false;
    anniversaries.clear();
    description.clear();
    photoType.clear();
    if (!photo.isEmpty())
        photo.clear();
    photoUrl.clear();
    organization.clear();
    title.clear();
    otherTags.clear();
    unknownTags.clear();
    originalFormat.clear();
    version.clear();
    subVersion.clear();
    // maybe not needed:
    prefPhone.clear();
    prefEmail.clear();
}

bool ContactItem::swapNames()
{
    if (names.isEmpty())
        return false;
    if (names.count()==1) names.push_back("");
    QString buffer = names[0];
    names[0] = names[1];
    names[1] = buffer;
    return true;
}

void ContactItem::calculateFields()
{
    // Visible name depend of filled lields
    if (!fullName.isEmpty())
        visibleName = fullName;
    else if (!names.isEmpty())
        visibleName = formatNames();
    else if (!organization.isEmpty())
        visibleName = organization;
    else if (!description.isEmpty())
        visibleName = description;
    else if (!emails.isEmpty())
        visibleName = emails[0].address;
    else if (!phones.isEmpty())
        visibleName = phones[0].number;
    else // WTF???
        visibleName = QObject::tr("Strange empty contact");
    // First or preferred phone number
    prefPhone.clear();
    if (phones.count()>0) {
        prefPhone = phones[0].number;
        for (int i=0; i<phones.count();i++) {
            if (phones[i].tTypes.contains("pref", Qt::CaseInsensitive))
                prefPhone = phones[i].number;
        }
    }
    // First or preferred email
    prefEmail.clear();
    if (emails.count()>0) {
        prefEmail = emails[0].address;
        for (int i=0; i<emails.count(); i++)
            if (emails[i].emTypes.contains("pref", Qt::CaseInsensitive))
                prefEmail = emails[i].address;
    }
}

QString ContactItem::formatNames()
{
    QString res = names[0]; // Last name
    if (names.count()>1) // First name
        res += " " + names[1];
    if (names.count()>2) // Middle name
        res += " " + names[2];
    if (names.count()>3) // Honorific Prefixes
        res = names[3] + " " + visibleName;
    if (names.count()>4) // Honorific Suffixes, rank, degree
        res += ", " + names[2];
    return res;
}

ContactList::ContactList()
{
}

int ContactList::findById(const QString &idValue)
{
    for(int i=0; i<count(); i++)
        if ((*this)[i].id==idValue)
            return i;
    return -1;
}

TagValue::TagValue(const QString& _tag, const QString& _value)
    :tag(_tag), value(_value)
{}

Phone::StandardTypes Phone::standardTypes;
Email::StandardTypes Email::standardTypes;
