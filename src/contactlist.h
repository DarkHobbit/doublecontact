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

#include <QDateTime>
#include <QStringList>
#include "globals.h"

struct TagValue {
    QString tag, value;
    TagValue(const QString& _tag, const QString& _value);
};

struct Phone {
    QString number;
    QStringList tTypes; // some phones may allow create any tel type (not RFC, but...)
    // standart types
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
    } standardTypes;
};

struct Email {
    QString address;
    QStringList emTypes;  // according RFC 2426, may be non-standard
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
};

struct ContactItem {
    QString fullName;
    QStringList names;
    QList<Phone> phones;
    QList<Email> emails;
    DateItem birthday;
    QList<DateItem> anniversaries;
    QString description;
    // TODO address
    // Format internals
    QString originalFormat;
    QString version;
    QList<TagValue> unknownTags; // specific tags for any file format, i.e. vcf
    // Calculated fields for higher perfomance
    QString prefPhone, prefEmail;
    void clear();
    bool swapNames();
    void calculateFields();
};

class ContactList : public QList<ContactItem>
{
public:
    ContactList();
};

#endif // CONTACTLIST_H
