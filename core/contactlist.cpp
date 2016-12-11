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

// Rules for phone number internationalization
struct CountryRule{
    QString country, nPrefix, iPrefix;
} countryRules [COUNTRY_RULES_COUNT] ={
    // Here we use _national_ names of countries
    // If list will expanded, may be port it into separate list, such as languages in LanguageManager
    {QString::fromUtf8("Беларусь"), "8", "+375"},
    {QString::fromUtf8("Россия"),   "8", "+7"},
    {QString::fromUtf8("Україна"),  "0", "+380"}
};

QString Phone::expandNumber(int countryRule) const
{
    return expandNumber(value, countryRule);
}

QString Phone::expandNumber(const QString &number, int countryRule)
{
    QString res = number;
    if (res.startsWith(countryRules[countryRule].nPrefix)) // for example, 8 -> +7 for Russia
            res = res.replace(0, 1, countryRules[countryRule].iPrefix);
    return res;
}

bool Phone::operator ==(const Phone &p)
{
    return (value==p.value && types==p.types);
}

QStringList Phone::availableCountryRules()
{
    QStringList rules;
    for (int i=0; i<COUNTRY_RULES_COUNT; i++) {
        CountryRule& rule = countryRules[i];
        rules << QString("%1 (%2 -> %3)")
            .arg(rule.country).arg(rule.nPrefix).arg(rule.iPrefix);
    }
    return rules;
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
    return (value==e.value && types==e.types);
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
    sortString.clear();
    description.clear();
    photoType.clear();
    if (!photo.isEmpty())
        photo.clear();
    photoUrl.clear();
    addrHome.clear();
    addrWork.clear();
    organization.clear();
    title.clear();
    otherTags.clear();
    unknownTags.clear();
    originalFormat.clear();
    version.clear();
    subVersion.clear();
    prefPhone.clear();
    prefEmail.clear();
    nickName.clear();
    url.clear();
    jabberName.clear();
    icqName.clear();
    skypeName.clear();
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

bool ContactItem::intlPhonePrefix(int countryRule)
{
    int res = false;
    for (int i=0; i<phones.count();i++) {
        QString newNumber = phones[i].expandNumber(countryRule);
        if (newNumber!=phones[i].value)
            res = true;
        phones[i].value = newNumber;
    }
    return res;
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
        visibleName = emails[0].value;
    else if (!phones.isEmpty())
        visibleName = phones[0].value;
    else if (!sortString.isEmpty())
        visibleName = sortString;
    else if (!nickName.isEmpty())
        visibleName = nickName;
    else // WTF???
        visibleName = QObject::tr("Strange empty contact");
    // First or preferred phone number
    prefPhone.clear();
    if (phones.count()>0) {
        prefPhone = phones[0].value;
        for (int i=0; i<phones.count();i++) {
            if (phones[i].types.contains("pref", Qt::CaseInsensitive))
                prefPhone = phones[i].value;
        }
    }
    // First or preferred email
    prefEmail.clear();
    if (emails.count()>0) {
        prefEmail = emails[0].value;
        for (int i=0; i<emails.count(); i++)
            if (emails[i].types.contains("pref", Qt::CaseInsensitive))
                prefEmail = emails[i].value;
    }
}

QString ContactItem::formatNames() const
{
    // We don't use QStringList::join
    // because field order is'nt completely regular
    QString res = names[0]; // Last name
    if (names.count()>1) {// First name
        if (!res.isEmpty())
            res += " ";
        res += names[1];
    }
    if (names.count()>2) {// Middle name
        if (!res.isEmpty())
            res += " ";
        res += " " + names[2];
    }
    if (names.count()>3) // Honorific Prefixes
        res = names[3] + " " + res;
    if (names.count()>4) // Honorific Suffixes, rank, degree
        res += ", " + names[4];
    return res;
}

void ContactItem::reverseFullName()
{
    int sPos = fullName.indexOf(" ");
    if (sPos!=-1)
        fullName = fullName.right(fullName.length()-sPos-1)
           + " " + fullName.left(sPos);
}

void ContactItem::dropFinalEmptyNames()
{
    while (names.last().isEmpty()) {
        names.removeLast();
        if (names.isEmpty()) break;
    }
}

bool ContactItem::similarTo(const ContactItem &pair, int priorityLevel)
{
    // TODO set options for various criter.
    switch (priorityLevel) {
        case 1:
        // Phones
        foreach (const Phone& thisPhone, phones)
            foreach (const Phone& pairPhone, pair.phones)
                if (thisPhone.expandNumber(gd.defaultCountryRule)==pairPhone.expandNumber(gd.defaultCountryRule))
                    return true;
        // Emails
        foreach (const Email& thisEmail, emails)
            foreach (const Email& pairEmail, pair.emails)
                if (thisEmail.value.toUpper()==pairEmail.value.toUpper())
                    return true;
        break;
        case 2:
        if (id.length()>4 && id==pair.id)
            return true;
        break;
        case 3:
        if (!addrHome.isEmpty() && addrHome==pair.addrHome)
            return true;
        if (!addrWork.isEmpty() && addrWork==pair.addrWork)
            return true;
        break;
        case 4:
        if (!fullName.isEmpty() && fullName==pair.fullName)
            return true;
        if ((names.count()>1) && (pair.names.count()>1) && (!names[0].isEmpty()) && (!names[1].isEmpty())) {
            // 2 reversed names equals
            if (names[0].toUpper()==pair.names[1].toUpper() && names[1].toUpper()==names[0].toUpper())
                return true;
            // 2 names equals
            if (names[0].toUpper()==pair.names[0].toUpper() && names[1].toUpper()==names[1].toUpper())
                return true;
            // Initials?..
        }
        break;
        case 5:
        if (!nickName.isEmpty() && nickName==pair.nickName)
            return true;
        if (organization==pair.organization) return false; //?
        break;
    default:
        break;
    }
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
    if (sortString!=pair.sortString) return false;
    if (description!=pair.description) return false;
    if (photoType!=pair.photoType) return false;
    if (photo!=pair.photo) return false;
    if (photoUrl!=pair.photoUrl) return false;
    if (organization!=pair.organization) return false;
    if (title!=pair.title) return false;
    if (!(addrHome==pair.addrHome)) return false;
    if (nickName!=pair.nickName) return false;
    if (url!=pair.url) return false;
    if (jabberName!=pair.jabberName) return false;
    if (icqName!=pair.icqName) return false;
    if (skypeName!=pair.skypeName) return false;
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
            for (int j=1; j<=MAX_COMPARE_PRIORITY_LEVEL; j++) {
                bool pairFound = false;
                for(int k=0; k<pairList.count(); k++) {
                    ContactItem& candidate = pairList[k];
                    if (item.similarTo(candidate, j)) {
                        pairFound = true;
                        item.pairState = ContactItem::PairSimilar;
                        item.pairItem = &candidate;
                        item.pairIndex = k;
                        candidate.pairItem = &item;
                        candidate.pairState = ContactItem::PairSimilar;
                        candidate.pairIndex = i;
                        break;
                    }
                }
                if (pairFound) break;
            }
    }
}

void ContactList::clear()
{
    QList<ContactItem>::clear();
    extra.clear();
}

QString ContactList::statistics()
{
    int phoneCount = 0;
    int emailCount = 0;
    int addrCount = 0;
    int bdayCount = 0;
    foreach (const ContactItem& item, *this) {
        phoneCount += item.phones.count();
        emailCount += item.emails.count();
        if (!item.addrHome.isEmpty())
            addrCount++;
        if (!item.addrWork.isEmpty())
            addrCount++;
        if (!item.birthday.isEmpty())
            bdayCount++;
    }
    return QObject::
        tr("%1 records\n%2 phones\n%3 emails\n%4 addresses\n%5 birthdays\n%6 calls\n%7 SMS\n%8 archived SMS\n%9 %10")
        .arg(count()).arg(phoneCount).arg(emailCount).arg(addrCount).arg(bdayCount)
        .arg(extra.calls.count()).arg(extra.SMS.count()).arg(extra.SMSArchive.count())
        .arg(extra.model).arg(extra.timeStamp);
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
        s = value.date().toString(gd.dateFormat);
        break;
    }
    if (hasTime) {
        switch (format) {
        case DateItem::ISOBasic:
            if (!gd.skipTimeFromDate)
                s += value.time().toString("Thhmmss");
            break;
        case DateItem::ISOExtended:
            if (!gd.skipTimeFromDate)
                s += value.time().toString("Thh:mm:ss");
            break;
        default:
            s += " " + value.time().toString(gd.timeFormat);
            break;
        }
        if (hasTimeZone && !gd.skipTimeFromDate) // TODO search Xamples with positive TZ hour and short form
            s += QString("%1:%2").arg(zoneHour).arg(zoneMin);
    }
    return s;
}

bool PostalAddress::operator ==(const PostalAddress &a)
{
    return
        paTypes==a.paTypes
     && offBox==a.offBox && extended==a.extended
            && street==a.street && city==a.city && region==a.region && postalCode==a.postalCode && country==a.country;
}

void PostalAddress::clear()
{
    paTypes.clear();
    offBox.clear();
    extended.clear();
    street.clear();
    city.clear();
    region.clear();
    postalCode.clear();
    country.clear();
}

QString PostalAddress::toString() const
{
    return offBox + ", " + extended
            + ", " + street + " st., " + city + ", " + region
            + ", " + postalCode + ", " + country;
    // TODO make localized output
}

bool PostalAddress::isEmpty() const
{
    return offBox.isEmpty() && extended.isEmpty()
        && street.isEmpty() && city.isEmpty() && region.isEmpty()
        && postalCode.isEmpty() && country.isEmpty();
}

PostalAddress::StandardTypes::StandardTypes()
{
    fill();
}

void PostalAddress::StandardTypes::fill()
{
    clear();
    displayValues.clear();
    // Types according RFC 2426
    (*this)["home"] = QObject::tr("Home");
    (*this)["work"] = QObject::tr("Work");
    (*this)["pref"] = QObject::tr("Preferable");
    (*this)["dom"] = QObject::tr("Domestic");
    (*this)["intl"] = QObject::tr("International");
    (*this)["postal"] = QObject::tr("Postal");
    (*this)["parcel"] = QObject::tr("Parcel");
    displayValues
        << (*this)["home"]  << (*this)["work"]
        << (*this)["pref"] << (*this)["dom"]
        << (*this)["intl"] << (*this)["postal"] << (*this)["parcel"];
}

void MPBExtra::clear()
{
    model.clear();
    timeStamp.clear();
    organizer.clear();
    notes.clear();
    SMS.clear();
    SMSArchive.clear();
    calls.clear();
}
