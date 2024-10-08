/* Double Contact
 *
 * Module: CSV file profile for Sylpheed Email Client (checked on Sylpheed 3.7)
 *
 * Copyright 2024 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "sylpheedprofile.h"


SylpheedProfile::SylpheedProfile()
{
    _name = QObject::tr("Sylpheed Email Client");
    _hasHeader = true;
    _charSet = "UTF-8";
    _hasBOM = false;
    _quotingPolicy = CSVProfileBase::QuoteIfNeed;
    _lineEnding = CSVProfileBase::LFEnding;
}

bool SylpheedProfile::detect(const QStringList &header) const
{
    return (header.count()>=7 && header.contains("Email"));
}

bool SylpheedProfile::importRecord(const QStringList &row, ContactItem &item, QStringList&, QString& fatalError)
{
    if (row.count()<7) {
        fatalError = S_CSV_ROW_TOO_SHORT.arg(row.join(","));
        return false;
    }
    item.names << row[1] << row[0];
    item.fullName = row[2];
    item.nickName = row[3];
    // Email
    QString emailType = "pref";
    // Use col 6 (Alias) as email type, if present
    if (present(row, 6))
        emailType = row[6];
    if (present(row, 4))
        item.emails << Email(row[4], emailType);
    // Description
    // (Actually, col 5 (Remarks) set for current email, not contact
    // but vCard not support it, and we write it into contact notes)
    if (present(row, 5))
        item.description = row[5];
    return true;
}

QStringList SylpheedProfile::makeHeader()
{
    return QStringList()
            << "First Name"
            << "Last Name"
            << "Display Name"
            << "Nick Name"
            << "E-Mail Address"
            << "Remarks"
            << "Alias";
}

bool SylpheedProfile::exportRecord(QStringList &row, const ContactItem &item, QStringList &errors)
{
    // In this profile we shall export each email as separate record
    // Contacts without emails WILL NOT saved, and program will write warning about it
    if (item.emails.isEmpty()) {
        errors << QObject::tr("Record without emails with not saved: ")
                  + item.makeGenericName();
        return false;
    }
    foreach (const Email& m, item.emails) {
        // Names
        row << saveNamePart(item, 1);
        row << saveNamePart(item, 0);
        LOSS_DATA(S_MIDDLE_NAME, item.names.count()>2); // Second name
        row << item.fullName << item.nickName
        // Emails, description
            << m.value << item.description << m.types.first();
        nextRow(row);
    }
    // loss other data
    LOSS_DATA(S_PHONE, !item.phones.isEmpty());
    LOSS_DATA(S_BDAY, !item.birthday.isEmpty());
    LOSS_DATA(S_ANN, !item.anniversary.isEmpty());
    LOSS_DATA(S_DESC, item.description);
    LOSS_DATA(S_PHOTO, !item.photo.isEmpty());
    LOSS_DATA(S_ORG, item.organization);
    LOSS_DATA(S_TITLE, item.title);
    LOSS_DATA(S_ADDR, !item.addrs.isEmpty());
    LOSS_DATA(S_URL, item.url);
    FileFormat::lossData(errors, item.visibleName, S_IM, !item.ims.isEmpty());
    return true;
}
