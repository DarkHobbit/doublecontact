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
#include <QFlags>
#include <QList>
#include <QString>
#include <QStringList>

struct TagValue {
    QString tag, value;
    TagValue(const QString& _tag, const QString& _value);
};

struct Phone {
    enum TelTypeFlag { // according RFC 2426
      Home  = 0x0001,
      Msg   = 0x0002,
      Work  = 0x0004,
      Pref  = 0x0008,
      Voice = 0x0010,
      Fax   = 0x0020,
      Cell  = 0x0040,
      Video = 0x0080,
      Pager = 0x0100,
      BBS   = 0x0200,
      Modem = 0x0400,
      Car   = 0x0800,
      ISDN  = 0x1000,
      PCS   = 0x2000,
      All   = 0x3FFF
    };
    Q_DECLARE_FLAGS(TelType, TelTypeFlag)
    QString number;
    TelType tType;
    QStringList typeToStrList() const;
    QString typeToString() const;
    bool typeFromString(const QString& s);
    // calculated fields for higher perfomance
    bool isMixed;
    void calculateFields();
};

extern const struct Phone fullPhoneFlagSet;
const short maxTelTypeFlagCount = 14;

struct Email {
    QString address;
    bool preferred;
    QStringList emTypes;  // according RFC 2426, may be non-standard
};

struct ContactItem {
    QString fullName;
    QStringList names;
    QList<Phone> phones;
    QList<Email> emails;
    QDateTime birthDay; // TODO maybe add field from timezone, if it was in vcf?
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
