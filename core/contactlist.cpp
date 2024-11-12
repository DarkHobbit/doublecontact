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

#include <algorithm>
#include <QObject>
#include "contactlist.h"
#include "corehelpers.h"

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

TypedDataItem::~TypedDataItem()
{}

template<class T>
const T* TypedDataItem::findByType(const QList<T> &list, const QString &itemType)
{
    foreach (const T& item, list) {
        if (item.types.contains(itemType, Qt::CaseInsensitive))
            return &item;
    }
    return 0;
}

QStringList TagList::findByName(const QString &tagName) const
{
    QStringList res;
    foreach (const TagValue& pair, *this)
        if (pair.tag.startsWith(tagName, Qt::CaseInsensitive))
            res << pair.value;
    return res;
}

int TagList::findOneByName(const QString &tagName) const
{
    for(int i=0; i<count(); i++)
        if ((*this)[i].tag.toUpper()==tagName)
            return i;
    return -1;
}

QString TypedStringItem::toString(bool) const
{
    return value;
}

Phone::Phone()
{}

Phone::Phone(const QString &_value, const QString &type1, const QString &type2)
{
    value = _value;
    if (!type1.isEmpty())
        types << type1;
    if (!type2.isEmpty())
        types << type2;
}

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

bool Phone::operator ==(const Phone &p) const
{
    //std::cout << types.join(";").toLocal8Bit().data() << " " << p.types.join(";").toLocal8Bit().data() << std::endl;
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
    (*this)["callback"] = QObject::tr("CALLBACK"); // Samsung A50
    displayValues
        << (*this)["home"]  << (*this)["work"] << (*this)["cell"]
        << (*this)["pref"] << (*this)["voice"]
        << (*this)["msg"] << (*this)["fax"] << (*this)["video"]
        << (*this)["pager"] << (*this)["bbs"]
        << (*this)["modem"] << (*this)["car"]
        << (*this)["isdn"] << (*this)["pcs"]
        << (*this)["callback"];
}

Email::Email()
{}

Email::Email(const QString &_value, const QString &type1, const QString &type2)
{
    value = _value;
    if (!type1.isEmpty())
        types << type1;
    if (!type2.isEmpty())
        types << type2;
}

bool Email::operator ==(const Email &e) const
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

Messenger::Messenger()
{}

Messenger::Messenger(const QString &_value, const QString &type1, const QString &type2)
{
    value = _value;
    if (!type1.isEmpty())
        types << type1;
    if (!type2.isEmpty())
        types << type2;
}

bool Messenger::operator ==(const Messenger &m) const
{
    return (value==m.value && types==m.types);
}

Messenger::StandardTypes::StandardTypes()
{
    fill();
}

void Messenger::StandardTypes::fill()
{
    clear();
    displayValues.clear();
    // Types according RFC 6350 and more
    (*this)["xmpp"] = "Jabber";
    (*this)["sip"] = "SIP";
    (*this)["icq"] = "ICQ";
    (*this)["skype"] = "Skype";
    (*this)["pref"] = QObject::tr("Preferable");
    displayValues
            << (*this)["xmpp"] << (*this)["sip"]
            << (*this)["icq"] << (*this)["skype"]
            << (*this)["pref"];
}

void ContactItem::clear()
{
    id.clear();
    idType = "UID";
    fullName.clear();
    names.clear();
    phones.clear();
    emails.clear();
    birthday.clear();
    anniversary.clear();
    sortString.clear();
    description.clear();
    photo.clear();
    addrs.clear();
    groups.clear();
    organization.clear();
    title.clear();
    role.clear();
    otherTags.clear();
    unknownTags.clear();
    originalFormat.clear();
    version.clear();
    subVersion.clear();
    lastRev.clear();
    prefPhone.clear();
    prefEmail.clear();
    prefIM.clear();
    visibleName.clear();
    allPhones.clear();
    homePhone.clear();
    workPhone.clear();
    cellPhone.clear();
    nickName.clear();
    url.clear();
    ims.clear();
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

bool ContactItem::joinNames()
{
    bool res = false;
    dropFinalEmptyNames();
    QString unitedName = names.join(" ");
    names.clear();
    names << unitedName;
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
    // Visible name (depend of filled fields)
    visibleName = makeGenericName();
    // Phones
    prefPhone.clear();
    allPhones.clear();
    homePhone.clear();
    workPhone.clear();
    cellPhone.clear();
    if (phones.count()>0) {
        prefPhone = phones[0].value;
        for (int i=0; i<phones.count();i++) {
            if (phones[i].types.contains("pref", Qt::CaseInsensitive))
                prefPhone = phones[i].value;
            if (!allPhones.isEmpty())
                allPhones += ", ";
            allPhones += phones[i].value;
            if (phones[i].types.contains("home", Qt::CaseInsensitive)) {
                if (!homePhone.isEmpty())
                    homePhone += ", ";
                homePhone += phones[i].value;
            }
            if (phones[i].types.contains("work", Qt::CaseInsensitive)) {
                if (!workPhone.isEmpty())
                    workPhone += ", ";
                workPhone += phones[i].value;
            }
            if (phones[i].types.contains("cell", Qt::CaseInsensitive)) {
                if (!cellPhone.isEmpty())
                    cellPhone += ", ";
                cellPhone += phones[i].value;
            }
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
    // First or preferred IM
    prefIM.clear();
    if (ims.count()>0) {
        prefIM = ims[0].value;
        for (int i=0; i<ims.count(); i++)
            if (ims[i].types.contains("pref", Qt::CaseInsensitive))
                prefIM = ims[i].value;
    }
    // Type sorting and lowercasing for correct compare
    sortTypes(phones);
    sortTypes(emails);
    sortTypes(addrs);
    sortTypes(ims);
}

template<class T>
void ContactItem::sortTypes(QList<T> &values)
{
    for (int i=0; i<values.count(); i++) {
        QStringList& types = values[i].types;
        StandardTypes& sTypes = values[i].standardTypes;
        for (int j=0; j<types.count(); j++) {
            bool isStandard;
            sTypes.translate(types[j], &isStandard);
            if (isStandard)
                types[j] = types[j].toLower();
        }
        types.sort();
    }
}

QString ContactItem::formatNames() const
{
    if (names.isEmpty())
        return makeGenericName();
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

QString ContactItem::makeGenericName() const
{
    QString res;
    // Visible name depend of filled fields
    if (!fullName.isEmpty())
        res = fullName;
    else if (!names.isEmpty())
        res = formatNames();
    else if (!organization.isEmpty())
        res = organization;
    else if (!role.isEmpty())
        res = role;
    else if (!description.isEmpty())
        res = description;
    else if (!emails.isEmpty())
        res = emails[0].value;
    else if (!phones.isEmpty())
        res = phones[0].value;
    else if (!sortString.isEmpty())
        res = sortString;
    else if (!nickName.isEmpty())
        res = nickName;
    else // WTF???
        res = QObject::tr("Strange empty contact");
    return res;
}

void ContactItem::parseFullName()
{
    names = fullName.split(" ");
    while (names.count()>MAX_NAMES)
        names.removeLast();
    for (int i=0; i<names.count(); i++)
        if (names[i].right(1)==",")
            names[i].remove(names[i].count()-1);
}

void ContactItem::reverseFullName()
{
    // TODO now works for two components
    // modify for three, or add in documentation and/or in faq
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

void ContactItem::formatPhones(const QString &templ)
{
    for(int i=0; i<phones.count(); i++) {
        QString src = phones[i].value;
        QString res = "";
        int srcPos = 0;
        int templPos = 0;
        // "+" is significant char, it ALWAYS transferred from source, if present
        if (src.startsWith("+")) {
            res += "+";
            srcPos++;
        }
        if (templ.startsWith("+"))
            templPos++;
        while (templPos<templ.length() && srcPos<src.length())
        {
            if (templ[templPos].toUpper()=='N') {
                while (srcPos<src.length() && !src[srcPos].isDigit())
                    srcPos++;
                if (srcPos<src.length()) {
                    res += src[srcPos];
                    srcPos++;
                }
                else break;
            }
            else
                res += templ[templPos];
            templPos++;
        }
        // If source longer than template...
        while (srcPos<src.length()) {
            if (src[srcPos].isDigit())
                res += src[srcPos];
            srcPos++;
        }
        phones[i].value = res;
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
        // Messengers
        foreach (const Messenger& thisIM, ims)
            foreach (const Messenger& pairIM, pair.ims)
                if (thisIM.value.toUpper()==pairIM.value.toUpper())
                    return true;
        break;
        case 2:
        if (id.length()>4 && id==pair.id)
            return true;
        break;
        case 3:
        foreach (const PostalAddress& thisAddr, addrs)
            foreach (const PostalAddress& pairAddr, pair.addrs)
                if (thisAddr==pairAddr)
                    return true;
        break;
        case 4:
        if (!fullName.isEmpty() && fullName==pair.fullName)
            return true;
        if ((names.count()>1) && (pair.names.count()>1) && (!names[0].isEmpty()) && (!names[1].isEmpty())) {
            // 2 reversed names equals
            if (names[0].toUpper()==pair.names[1].toUpper() && names[1].toUpper()==pair.names[0].toUpper())
                return true;
            // 2 names equals
            if (names[0].toUpper()==pair.names[0].toUpper() && names[1].toUpper()==pair.names[1].toUpper())
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
    if (!(anniversary==pair.anniversary)) return false;
    if (sortString!=pair.sortString) return false;
    if (description!=pair.description) return false;
    if (!(photo==pair.photo)) return false;
    if (organization!=pair.organization) return false;
    if (title!=pair.title) return false;
    if (role!=pair.role) return false;
    if (addrs!=pair.addrs) return false;
    if (nickName!=pair.nickName) return false;
    if (url!=pair.url) return false;
    if (ims!=pair.ims) return false;
    // Here strongly add ALL new
    return true;
}

QString ContactItem::nameComponent(int compNum)
{
    QString res = "";
    switch (compNum) {
    case 0:
        res = S_LAST_NAME;
        break;
    case 1:
        res = S_FIRST_NAME;
        break;
    case 2:
        res = S_MIDDLE_NAME;
        break;
    case 3:
        res = S_NAME_PREFIXES;
        break;
    default:
        res = S_NAME_SUFFIXES;
        break;
    }
    return res;
}

const QString ContactItem::findIMByType(const QString &itemType) const
{
    const Messenger* im = TypedDataItem::findByType(ims, itemType);
    if (im)
        return im->value;
    else
        return "";
}

bool ContactItem::operator <(const ContactItem &pair) const
{
    return actualSortString < pair.actualSortString;
}

ContactList::ContactList()
{
}

void ContactList::clear()
{
    QList<ContactItem>::clear();
    emptyGroups.clear();
    extra.clear();
    originalPath.clear();
    originalProfile.clear();
}

void ContactList::sort(ContactList::SortType sortType)
{
    for (int i=0; i<count(); i++) {
        ContactItem& c = (*this)[i];
        QString lastName = !c.names.isEmpty() ? c.names[0] : "";
        QString firstName = c.names.count()>1  ? c.names[1] : "";
        switch (sortType) {
        case SortBySortString:
            c.actualSortString = c.sortString;
            break;
        case SortByLastName:
            c.actualSortString = lastName + " " + firstName;
            break;
        case SortByFirstName:
            c.actualSortString = firstName + " " + lastName;
            break;
        case SortByFullName:
            c.actualSortString = c.fullName;
            break;
        case SortByNick:
            c.actualSortString = c.nickName;
            break;
        case SortByGroup:
            c.actualSortString = c.groups.join(", ");
        }
    }
    sortP(this);
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

QMap<QString, int> ContactList::groupStat() const
{
    QMap<QString, int> res;
    foreach (const QString& g, emptyGroups)
        res[g] = 0;
    foreach (const ContactItem& item, *this)
        foreach(const QString& g, item.groups)
            res[g]++;
    return res;
}

bool ContactList::hasGroups() const
{
    if (!emptyGroups.isEmpty())
        return true;
    foreach (const ContactItem& item, *this)
        if (!item.groups.isEmpty())
            return true;
    return false;
}

bool ContactList::hasGroup(const QString &group) const
{
    if (emptyGroups.contains(group))
        return true;
    foreach (const ContactItem& item, *this)
        if (item.groups.contains(group))
            return true;
    return false;
}

bool ContactList::addGroup(const QString &group)
{
    if (hasGroup(group))
        return false;
    emptyGroups << group;
    sortO(emptyGroups);
    return true;
}

bool ContactList::renameGroup(const QString &oldName, const QString &newName)
{
    if (hasGroup(newName))
        return false;
    if (emptyGroups.contains(oldName)) {
        emptyGroups.removeOne(oldName);
        emptyGroups << newName;
        sortO(emptyGroups);
    }
    else {
        for (int i=0; i<this->count(); i++) {
            ContactItem& item = (*this)[i];
            if (item.groups.contains(oldName)) {
                item.groups.removeOne(oldName);
                item.groups << newName;
                sortO(item.groups);
            }
        }
    }
    return true;
}

bool ContactList::removeGroup(const QString &group)
{
    bool changed = emptyGroups.removeOne(group);
    for (int i=0; i<this->count(); i++)
        changed = changed || ((*this)[i].groups.removeOne(group));
    return changed;
}

QStringList ContactList::contactsInGroup(const QString &group)
{
    QStringList contacts;
    foreach (const ContactItem& item, *this)
        if (item.groups.contains(group))
            contacts << item.visibleName;
    return contacts;
}

void ContactList::includeToGroup(const QString &group, ContactItem &item)
{
    if (!item.groups.contains(group)) {
        item.groups << group;
        sortO(item.groups);
    }
    if (emptyGroups.contains(group))
        emptyGroups.removeOne(group);
}

void ContactList::excludeFromGroup(const QString &group, ContactItem &item)
{
    item.groups.removeOne(group);
    bool groupWillBeEmpty = true;
    foreach(const ContactItem &cand, *this)
        if (cand.groups.contains(group)) {
            groupWillBeEmpty = false;
            break;
        }
    if (groupWillBeEmpty) {
        emptyGroups << group;
        sortO(emptyGroups);
    }
}

void ContactList::mergeGroups(const QString &unitedGroup, const QString &mergedGroup)
{
    for (int i=0; i<this->count(); i++) {
        ContactItem& item = (*this)[i];
        if (item.groups.contains(mergedGroup)) {
            item.groups.removeOne(mergedGroup);
            if (!item.groups.contains(unitedGroup)) {
                item.groups << unitedGroup;
                sortO(item.groups);
            }
        }
    }
    emptyGroups.removeOne(mergedGroup);
    if (groupStat()[unitedGroup]==0) {
        emptyGroups << unitedGroup;
        sortO(emptyGroups);
    }
}

void ContactList::splitGroup(const QString &existGroup, const QString &newGroup, const QList<int> &movedIndicesInGroup)
{
    int index = -1;
    for (int i=0; i<this->count(); i++) {
        ContactItem& item = (*this)[i];
        if (item.groups.contains(existGroup)) { // Is contact in group?
            index++;
            if (movedIndicesInGroup.contains(index)) { // Is contact selected for move?
                item.groups.removeOne(existGroup);
                if (!item.groups.contains(newGroup)) {
                    item.groups << newGroup;
                    sortO(item.groups);
                }
            }
        }
    }
    QMap<QString, int> groups = groupStat();
    if (groups[existGroup]==0 && !emptyGroups.contains(existGroup)) {
        emptyGroups << existGroup;
        sortO(emptyGroups);
    }
    if (groups[newGroup]==0 && !emptyGroups.contains(newGroup)) {
        emptyGroups << newGroup;
        sortO(emptyGroups);
    }
}

QMap<QString, int> ContactList::nonStandardTagUsage() const
{
    QMap<QString, int> res;
    foreach (const ContactItem& item, *this) {
        foreach(const TagValue& tag, item.unknownTags)
            res[tag.tag]++;
        foreach(const TagValue& tag, item.otherTags)
            if (tag.tag.startsWith("X-", Qt::CaseInsensitive))
                res[tag.tag]++;
    }
    return res;
}

bool ContactList::massTagRemove(const QStringList &tagNames)
{
    bool res = false;
#if __cplusplus >= 201103L
    for (ContactItem& c: *this) {
#else
    // TODO check on Wheeze
    for (int i=0; i<count(); i++) {
        ContactItem& c = (*this)[i];
#endif
        foreach(const QString& tag, tagNames) {
            int index = c.unknownTags.findOneByName(tag);
            if (index>-1) {
                c.unknownTags.removeAt(index);
                res = true;
            }
            else {
                index = c.otherTags.findOneByName(tag);
                if (index>-1) {
                    c.otherTags.removeAt(index);
                    res = true;
                }
            }
        }
    }
    return res;
}

int ContactList::findById(const QString &idValue) const
{
    for(int i=0; i<count(); i++)
        if ((*this)[i].id==idValue)
            return i;
    return -1;
}

QString ContactList::statistics() const
{
    int phoneCount = 0;
    int emailCount = 0;
    int addrCount = 0;
    int bdayCount = 0;
    foreach (const ContactItem& item, *this) {
        phoneCount += item.phones.count();
        emailCount += item.emails.count();
        addrCount  += item.addrs.count();
        if (!item.birthday.isEmpty())
            bdayCount++;
    }
    QString res= QObject::tr("%1 records\n%2 phones\n%3 emails\n%4 addresses\n%5 birthdays")        
        .arg(count()).arg(phoneCount).arg(emailCount).arg(addrCount).arg(bdayCount);
#ifdef WITH_CALLS
    res += QObject::tr("\n%1 calls").arg(extra.calls.count());
#endif
#ifdef WITH_MESSAGES
    QString smsStat = QString("%1+%2+%3").arg(extra.vmsgSMS.count()).arg(extra.pduSMS.count()).arg(extra.binarySMS.count());
    QString archSmsStat = QString("%1+%2").arg(extra.vmsgSMSArchive.count()).arg(extra.pduSMSArchive.count());
    res += QObject::tr("\n%1 SMS\n%2 archived SMS").arg(smsStat).arg(archSmsStat);
#endif
    if (!extra.model.isEmpty())
        res += QObject::tr("\n\nmodel %1\nwritten %2\nIMEI %3\nfirmware %4\nphone language %5")
            .arg(extra.model).arg(extra.timeStamp.toString())
            .arg(extra.imei).arg(extra.firmware).arg(extra.phoneLang);
    return res;
}

#ifdef WITH_MESSAGES
bool ContactList::hasMessages()
{
    return
        (!extra.vmsgSMS.isEmpty() || !extra.pduSMS.isEmpty() || !extra.binarySMS.isEmpty()
        || !extra.vmsgSMSArchive.isEmpty() || !extra.pduSMSArchive.isEmpty());
}
#endif

#ifdef WITH_CALLS
bool ContactList::hasCalls()
{
    return !extra.calls.isEmpty();
}

void ContactList::updateCallHistory(const QStringList& droppedFullNames)
{
    if (extra.calls.isEmpty())
        return; // exclude slow map building for most formats
    NumberNameMap nNames(*this);
    for (int i=0; i<extra.calls.count(); i++) {
        CallInfo& call = extra.calls[i];
        QString newName = nNames.nameByNumber(call.number);
        if (newName == call.name)
            continue;
        if (newName.isEmpty()) {
            if (!droppedFullNames.contains(call.name))
                continue; // No remove, if not in list!
            // Setting empty newName may destroy special contact info
            // Also it work incorrect if default country rule wrong or missing
        }
        call.name = newName;
    }
}
#endif

bool ContactList::hasFiles()
{
    return !extra.files.isEmpty();
}

TagValue::TagValue(const QString& _tag, const QString& _value)
    :tag(_tag), value(_value), data(), isBinary(false)
{}

TagValue::TagValue(const QString &_tag, const QByteArray _data)
    :tag(_tag), value(), data(_data), isBinary(true)
{}

QString TagValue::text()
{
    return isBinary ? QObject::tr("BINARY DATA") : value;
}

DateItem::DateItem()
    :hasYear(true), hasTime(false), hasTimeZone(false), zoneHour(0), zoneMin(0)
{
    value = QDateTime();
}

DateItem::DateItem(const QDateTime &dateTime)
    :hasYear(true), hasTime(false), hasTimeZone(false), zoneHour(0), zoneMin(0)
{
    value = dateTime;
}

bool DateItem::operator ==(const DateItem &d) const
{
    if (hasTimeZone && ((zoneHour!=d.zoneHour) || (zoneMin!=d.zoneMin)))
        return false;
    return (value==d.value && hasTime==d.hasTime && hasTimeZone==d.hasTimeZone);
}

void DateItem::clear()
{
    value = QDateTime();
    hasYear = true;
    hasTime = false;
    hasTimeZone = false;
    zoneHour = 0;
    zoneMin = 0;
}

QString DateItem::toString(DateFormat format) const
{
    QString fs;
    switch (format) {
    case DateItem::ISOBasic:
        fs = hasYear ? "yyyyMMdd" : "--MMdd";
        break;
    case DateItem::ISOExtended:
        fs = hasYear ? "yyyy-MM-dd" : "--MM-dd";
        break;
    default:
        fs = hasYear ? gd.dateFormat : "d MMMM";
        break;
    }
    QString s = value.date().toString(fs);
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
        if (hasTimeZone && !gd.skipTimeFromDate)
            // TODO search Xamples with positive TZ hour and short form
            //s += QString("%1:%2").arg(zoneHour).arg(zoneMin);
            s += "Z"; // temp workaround
    }
    return s;
}

const QString ISO_EXT_TIME_FORMAT =
#if QT_VERSION >= 0x051000
        QString("yyyyMMddThhmmsst");
#else
        QString("yyyyMMddThhmmss");
#endif

QDateTime DateItem::readISOExtDateTimeWithZone(const QString &src)
{
#if QT_VERSION >= 0x051000
    return QDateTime::fromString(ISO_EXT_TIME_FORMAT);
#else
    QString sDate = src.left(ISO_EXT_TIME_FORMAT.length());
    QDateTime res = QDateTime::fromString(sDate, ISO_EXT_TIME_FORMAT);
    // TODO for Qt 5.2(???) - Qt 5.9.x extract timezone here
    // and manually add it to res
    // TODO for Qt4, if tail=src.mid(ISO_EXT_TIME_FORMAT.length())!='Z', warn
    return res;
#endif
}

QString DateItem::writeISOExtDateTimeWithZone(const QDateTime &src)
{
#if QT_VERSION >= 0x051000
    return src.toString(ISO_EXT_TIME_FORMAT);
#else
    QString res = src.toString(ISO_EXT_TIME_FORMAT);
    // TODO for Qt 5.2(???) - Qt 5.9.x add timezone here
    res += "Z";
    return res;
#endif
}

bool PostalAddress::operator ==(const PostalAddress &a) const
{
    return
        types==a.types
     && offBox==a.offBox && extended==a.extended
            && street==a.street && city==a.city && region==a.region && postalCode==a.postalCode && country==a.country;
}

void PostalAddress::clear()
{
    types.clear();
    offBox.clear();
    extended.clear();
    street.clear();
    city.clear();
    region.clear();
    postalCode.clear();
    country.clear();
}

QString PostalAddress::toString(bool humanReadable) const
{
    QString sep = humanReadable ? ", " : ";";
    QString s = offBox + sep + extended + sep + street
            + (humanReadable ? " st." : "")
            + sep + city + sep + region
            + sep + postalCode + sep + country;
    if (humanReadable)
        while (s.startsWith(sep))
            s.remove(0, sep.length());
    return s;
    // TODO make localized output
}

PostalAddress PostalAddress::fromString(const QString &src, const QStringList &_types)
{
    PostalAddress a;
    a.types = _types;
    QStringList parts = src.split(", ");
    if (parts.count()>0)
        a.offBox = parts[0];
    if (parts.count()>1)
        a.extended = parts[1];
    if (parts.count()>2) {
        a.street = parts[2];
        a.street.remove(" st.");
    }
    if (parts.count()>3)
        a.city = parts[3];
    if (parts.count()>4)
        a.region = parts[4];
    if (parts.count()>5)
        a.postalCode = parts[5];
    if (parts.count()>6)
        a.country = parts[6];
    return a;
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

bool Photo::operator ==(const Photo &p) const
{
    return pType==p.pType && url==p.url && data==p.data;
}

void Photo::clear()
{
    pType.clear();
    url.clear();
    if (!data.isEmpty())
        data.clear();
}

bool Photo::isEmpty() const
{
    return data.isEmpty() && url.isEmpty();
}

QString Photo::detectFormat() const
{
    QString format = "UNKNOWN";
    if (data.mid(0, 2).contains("\xFF\xD8"))
        format = "JPEG";
    else if (data.mid(1, 3).contains("PNG"))
        format = "PNG";
    return format;
}

Phone::StandardTypes Phone::standardTypes;
Email::StandardTypes Email::standardTypes;
PostalAddress::StandardTypes PostalAddress::standardTypes;
Messenger::StandardTypes Messenger::standardTypes;

NumberNameMap::NumberNameMap(const ContactList &list)
{
    // TODO move using country rule to options (here and in nameByNumber())
    // Collect contacts
    foreach(const ContactItem& c, list) {
        QString cName = c.makeGenericName();
        foreach(const Phone& p, c.phones) {
            //QString number = p.value;
            QString number = p.expandNumber(gd.defaultCountryRule);
            number.remove(" ").remove("-").remove("(").remove(")");
            (*this)[number] = cName;
        }
    }
}

QString NumberNameMap::nameByNumber(const QString &number)
{
    //return << (*this)[number];
    return (*this)[Phone::expandNumber(number, gd.defaultCountryRule)];
}

