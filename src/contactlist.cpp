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
#include <iostream>

QString Phone::expandNumber() const
{
    QString res = number;
    if (number.startsWith("8")) // Russia; TODO: make table for some countries!
            res = res.replace(0, 1, "+7");
    return res;
}

bool Phone::operator ==(const Phone &p)
{
    return (number==p.number && tTypes==p.tTypes);
}

Phone::StandardTypes::StandardTypes()
{
    fill();
}

void Phone::StandardTypes::fill()
{
    clear();
    displayValues.clear();
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

bool Email::operator ==(const Email &e)
{
    return (address==e.address && emTypes==e.emTypes);
}

Email::StandardTypes::StandardTypes()
{
    fill();
}

void Email::StandardTypes::fill()
{
    clear();
    displayValues.clear();
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
    birthday.clear();
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
    dropFinalEmptyNames();
    return true;
}

bool ContactItem::splitNames()
{
    bool res = false;
    dropFinalEmptyNames();
    for (int i=0; i<names.count(); i++) {
        int sPos = names[i].indexOf(" ");
        if (sPos!=-1) {
            names.push_back(names[i].mid(sPos+1));
            names[i] = names[i].left(sPos);
            res = true;
        }
    }
    return res;
}

bool ContactItem::dropSlashes()
{
    for (int i=0; i<names.count(); i++) {
        if (names[i].right(1)=="\\")
            names[i].remove(names[i].length()-1, 1);
        int len = names[i].length();
        if (len>2)
            if (names[i][len-2]=='/' && names[i][len-1].isDigit())
                names[i].remove(len-2, 2);
    }
    return true;
}

void ContactItem::calculateFields()
{
    // Visible name depend of filled fields
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
    // TODO by address
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

void ContactItem::dropFinalEmptyNames()
{
    while (names.last().isEmpty()) {
        names.removeLast();
        if (names.isEmpty()) break;
    }
}

bool ContactItem::similarTo(const ContactItem &pair)
{
    // TODO set options for various criter.
    if (id.length()>4 && id==pair.id)
        return true;
    if (!fullName.isEmpty() && fullName==pair.fullName)
        return true;
    if ((names.count()>1) && (pair.names.count()>1)) {
        // 2 names equals
        if (names[0].toUpper()==pair.names[1].toUpper() && names[1].toUpper()==names[0].toUpper())
            return true;
        // 2 reversed names equals
        if (names[0].toUpper()==pair.names[0].toUpper() && names[1].toUpper()==names[1].toUpper())
            return true;
        // Initials?..
    }
    // Phones
    foreach (const Phone& thisPhone, phones)
        foreach (const Phone& pairPhone, pair.phones)
            if (thisPhone.expandNumber()==pairPhone.expandNumber())
                return true;
    // Emails
    foreach (const Email& thisEmail, emails)
        foreach (const Email& pairEmail, pair.emails)
            if (thisEmail.address.toUpper()==pairEmail.address.toUpper())
                return true;
    if (organization==pair.organization) return false;
    //TODO address
    return false;
}

bool ContactItem::identicalTo(const ContactItem &pair)
{
    // TODO set options for various criter.
    if (fullName!=pair.fullName) return false;
    if (names!=pair.names) return false;
    if (phones!=pair.phones) return false;
    if (emails!=pair.emails) return false;
    if (!(birthday==pair.birthday)) return false;
    if (anniversaries!=pair.anniversaries) return false;
    if (description!=pair.description) return false;
    if (photoType!=pair.photoType) return false;
    if (photo!=pair.photo) return false;
    if (photoUrl!=pair.photoUrl) return false;
    if (organization!=pair.organization) return false;
    if (title!=pair.title) return false;
    //TODO address
    // Here strongly add ALL new
    return true;
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

void ContactList::compareWith(ContactList &pairList)
{
    for (int i=0; i<pairList.count(); i++)
        pairList[i].pairState = ContactItem::PairNotFound;
    for (int i=0; i<count(); i++) {
        ContactItem& item = (*this)[i];
        item.pairState = ContactItem::PairNotFound;
        item.pairItem = 0;
        // At first, search complete matching
        for(int j=0; j<pairList.count(); j++) {
            ContactItem& candidate = pairList[j];
            if (item.identicalTo(candidate)) {
                item.pairState = ContactItem::PairIdentical;
                item.pairItem = &candidate;
                item.pairIndex = j;
                candidate.pairItem = &item;
                candidate.pairState = ContactItem::PairIdentical;
                candidate.pairIndex = i;
                break;
            }
        }
        // If no identical records, search similar
        if (item.pairState==ContactItem::PairNotFound)
            for(int j=0; j<pairList.count(); j++) {
                ContactItem& candidate = pairList[j];
                if (item.similarTo(candidate)) {
                    item.pairState = ContactItem::PairSimilar;
                    item.pairItem = &candidate;
                    item.pairIndex = j;
                    candidate.pairItem = &item;
                    candidate.pairState = ContactItem::PairSimilar;
                    candidate.pairIndex = i;
                    break;
                }
            }
    }
}

TagValue::TagValue(const QString& _tag, const QString& _value)
    :tag(_tag), value(_value)
{}

Phone::StandardTypes Phone::standardTypes;
Email::StandardTypes Email::standardTypes;

bool DateItem::operator ==(const DateItem &d)
{
    if (hasTimeZone && ((zoneHour!=d.zoneHour) || (zoneMin!=d.zoneMin)))
        return false;
    return (value==d.value && hasTime==d.hasTime && hasTimeZone==d.hasTimeZone);
}

void DateItem::clear()
{
    value = QDateTime();
    hasTime = false;
    hasTimeZone = false;
    zoneHour = 0;
    zoneMin = 0;
}

QString DateItem::toString(DateFormat format) const
{
    QString s;
    switch (format) {
    case DateItem::ISOBasic:
        s = value.date().toString("yyyyMMdd");
        break;
    case DateItem::ISOExtended:
        s = value.date().toString("yyyy-MM-dd");
        break;
    default:
        s = value.date().toString(Qt::DefaultLocaleLongDate);
        break;
    }
    if (hasTime) {
        s += (format==DateItem::Local) ? " " : "T";
        switch (format) {
        case DateItem::ISOBasic:
            s += value.time().toString("Thhmmss");
            break;
        case DateItem::ISOExtended:
            s += value.time().toString("Thh:mm:ss");
            break;
        default:
            s += value.time().toString();
            break;
        }
        s += value.time().toString("Thhmmss");
        if (hasTimeZone) // TODO search Xamples with positive TZ hour and short form
            s += QString("%1:%2").arg(zoneHour).arg(zoneMin);
    }
    return s;
}
