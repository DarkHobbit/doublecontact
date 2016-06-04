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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QByteArray>
#include <QDateTime>
#include <QStringList>
#include "globals.h"

struct TagValue { // for non-editing ang unknown tags
    QString tag, value;
    TagValue(const QString& _tag, const QString& _value);
};

struct Phone {
    QString number;
    QStringList tTypes; // some devices & addressbooks may allow create any tel type (not RFC, but...)
    QString expandNumber() const; // international number representation from particular
    bool operator ==(const Phone& p);
    // standart types
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
    } standardTypes;
};

struct Email {
    QString address;
    QStringList emTypes;  // according RFC 2426, may be non-standard
    bool operator ==(const Email& e);
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
    } standardTypes;
};

struct DateItem { // Birthday and anniversaries
    QDateTime value; // date & time
    bool hasTime; // false if date only was in file
    bool hasTimeZone; // record contains TZ info
    short zoneHour, zoneMin; // TZ value
    bool operator ==(const DateItem& d);
    void clear();
};

struct ContactItem {
    QString fullName;
    QStringList names;
    QList<Phone> phones;
    QList<Email> emails;
    DateItem birthday;
    QList<DateItem> anniversaries;
    QString description;
    // Photo
    QString photoType; // URL, JPEG, PNG or unsupported, but stored value
    QByteArray photo;
    QString photoUrl;
    // Work
    QString organization, title;
    // TODO role, logo?
    // TODO address
    // Format internals
    QString id; // optional record unique id (udx Sequence, vcf X-IRMC-LUID, etc)
    QString originalFormat;
    QString version, subVersion;
    QList<TagValue> otherTags;   // Known but un-editing tags
    QList<TagValue> unknownTags; // specific tags for any file format, i.e. vcf
    // Calculated fields for higher perfomance
    QString visibleName, prefPhone, prefEmail;
    // Calculated fields for list comparison
    enum PairState {
        PairNotFound,
        PairSimilar,
        PairIdentical
    } pairState;
    ContactItem* pairItem;
    int pairIndex;
    // Aux methods
    void clear();
    bool swapNames();
    bool splitNames(); // TODO int index
    bool dropSlashes();
    void calculateFields();
    QString formatNames();
    bool similarTo(const ContactItem& pair);
    bool identicalTo(const ContactItem& pair);
};

class ContactList : public QList<ContactItem>
{
public:
    ContactList();
    int findById(const QString& idValue);
    void compareWith(ContactList& pairList);
};

#endif // CONTACTLIST_H
