/* Double Contact
 *
 * Module: CSV file profile for Osmo PIM
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include <QObject>
#include "osmoprofile.h"

// DoubleContact language and Osmo language must be identical
// else new phantom group will be created
// It's bad design of Osmo CSV format.
#define S_OSMO_NO_GROUP QObject::tr("None", "Osmo PIM marker for contacts without groups for YOUR language")

OsmoProfile::OsmoProfile()
{
    _name = "Osmo PIM";
    _hasHeader = true;
    _charSet = "UTF-8";
    _hasBOM = false;
    _quotingPolicy = CSVProfileBase::QuoteIfNeed;
    _lineEnding = CSVProfileBase::LFEnding;
}


bool OsmoProfile::detect(const QStringList &header) const
{
    return header.contains("First name") || header.contains("Last name");

}

bool OsmoProfile::parseHeader(const QStringList &header)
{
    foreach(const QString& col, header)
        columnIndexes[col] = header.indexOf(col);
    return !columnIndexes.isEmpty();
}

bool OsmoProfile::importRecord(const QStringList &row, ContactItem &item, QStringList &errors)
{
    // TODO m.b. need fatalError
    if (row.count()<4) {
        errors << S_CSV_ROW_TOO_SHORT.arg(row.join(","));
        return false;
    }
    // Group
    if (present(row, "Group")) {
        QString grName = value(row, "Group");
        if ((!grName.isEmpty()) && (grName!=S_OSMO_NO_GROUP))
            // TODO full group support
            item.otherTags << TagValue("CATEGORIES", row[0]);
    }
    // Names and dates
    item.names << value(row, "Last name") << value(row, "First name") << value(row, "Second name");
    item.nickName = value(row, "Nickname");
    if (present(row, "Tags"))
        item.unknownTags << TagValue("TAGS", value(row, "Tags"));
    // TODO check on invalid date
    item.birthday = QDateTime::fromString(value(row, "Birthday date"), "dd.mm.yyyy");
    if (present(row, "Name day date")) {
        item.anniversaries << QDateTime::fromString(value(row, "Name day date"), "dd.mm.yyyy");
        errors << QObject::tr("Name day loaded as anniversary, contact %1").arg(item.makeGenericName());
    }
    // Home address
    if (present(row, "Home address") || present(row, "Home postcode")
    || present(row, "Home city") || present(row, "Home state")
    || present(row, "Home country"))
    {
        PostalAddress addr;
        addr.types << "Home";
        addr.street = value(row, "Home address");
        addr.postalCode = value(row, "Home postcode");
        addr.city = value(row, "Home city");
        addr.region = value(row, "Home state");
        addr.country = value(row, "Home country");
        item.addrs << addr;
    }
    // Work
    item.organization = value(row, "Organization");
    item.title = value(row, "Department");
    if (!item.title.isEmpty())
        errors << QObject::tr("Department loaded as Job title");
    // Work address
    if (present(row, "Work address") || present(row, "Work postcode")
    || present(row, "Work city") || present(row, "Work state")
    || present(row, "Work country"))
    {
        PostalAddress addr;
        addr.types << "Work";
        addr.street = value(row, "Work address");
        addr.postalCode = value(row, "Work postcode");
        addr.city = value(row, "Work city");
        addr.region = value(row, "Work state");
        addr.country = value(row, "Work country");
        item.addrs << addr;
    }
    // Phones/fax
    if (present(row, "Fax"))
        item.phones << Phone(value(row, "Fax"), "fax");
    if (present(row, "Home phone"))
        item.phones << Phone(value(row, "Home phone"), "home");
    if (present(row, "Home phone 2"))
        item.phones << Phone(value(row, "Home phone 2"), "home");
    if (present(row, "Home phone 3"))
        item.phones << Phone(value(row, "Home phone 3"), "home");
    if (present(row, "Home phone 4"))
        item.phones << Phone(value(row, "Home phone 4"), "home");
    if (present(row, "Work phone"))
        item.phones << Phone(value(row, "Work phone"), "work");
    if (present(row, "Work phone 2"))
        item.phones << Phone(value(row, "Work phone 2"), "work");
    if (present(row, "Work phone 3"))
        item.phones << Phone(value(row, "Work phone 3"), "work");
    if (present(row, "Work phone 4"))
        item.phones << Phone(value(row, "Work phone 4"), "work");
    if (present(row, "Cell phone"))
        item.phones << Phone(value(row, "Cell phone"), "cell");
    if (present(row, "Cell phone 2"))
        item.phones << Phone(value(row, "Cell phone 2"), "cell");
    if (present(row, "Cell phone 3"))
        item.phones << Phone(value(row, "Cell phone 3"), "cell");
    if (present(row, "Cell phone 4"))
        item.phones << Phone(value(row, "Cell phone 4"), "cell");
    // E-Mail
    if (present(row, "E-Mail"))
        item.emails << Email(value(row, "E-Mail"), "pref");
    if (present(row, "E-Mail 2"))
        item.emails << Email(value(row, "E-Mail 2"), "pref");
    if (present(row, "E-Mail 3"))
        item.emails << Email(value(row, "E-Mail 3"), "pref");
    if (present(row, "E-Mail 4"))
        item.emails << Email(value(row, "E-Mail 4"), "pref");
    // Web
    item.url = value(row, "WWW");
    // TODO implement multiple urls for vCard 4.0, instead WWW2-WWW4
    if (present(row, "WWW2"))
        item.unknownTags << TagValue("WWW2", value(row, "WWW2"));
    if (present(row, "WWW3"))
        item.unknownTags << TagValue("WWW3", value(row, "WWW3"));
    if (present(row, "WWW4"))
        item.unknownTags << TagValue("WWW4", value(row, "WWW4"));
    // IMs
    if (present(row, "IM Gadu-Gadu"))
        item.ims << Messenger(value(row, "IM Gadu-Gadu"), "gadu-gadu");
    if (present(row, "IM Yahoo"))
        item.ims << Messenger(value(row, "IM Yahoo"), "Yahoo");
    if (present(row, "IM MSN"))
        item.ims << Messenger(value(row, "IM MSN"), "msn");
    if (present(row, "IM ICQ"))
        item.ims << Messenger(value(row, "IM ICQ"), "icq");
    if (present(row, "IM AOL"))
        item.ims << Messenger(value(row, "IM AOL"), "aim");
    if (present(row, "IM Jabber"))
        item.ims << Messenger(value(row, "IM Jabber"), "xmpp");
    if (present(row, "IM Skype"))
        item.ims << Messenger(value(row, "IM Skype"), "skype");
    if (present(row, "IM Tlen"))
        item.ims << Messenger(value(row, "IM Tlen"), "tlen");
    if (present(row, "Blog"))
        item.unknownTags << TagValue("BLOG", value(row, "Blog"));
    item.description = value(row, "Additional info");
    return true;
}

bool OsmoProfile::exportRecord(QStringList &row, const ContactItem &item, QStringList &errors)
{
    // Group TODO full group support
    QStringList categories = item.otherTags.findByName("CATEGORIES");
    if (categories.count()==0)
        row << S_OSMO_NO_GROUP;
    else
        row << categories.join(",");
    // Names
    row << saveNamePart(item, 1);
    row << saveNamePart(item, 0);
    row << saveNamePart(item, 2);
    // Nick, tags, birthday, name day
    row << item.nickName;
    row << item.unknownTags.findByName("TAGS").join(",");
    if (!item.birthday.isEmpty())
        row << item.birthday.value.toString("dd.mm.yyyy");
    if (!item.anniversaries.isEmpty()) {
        row << item.anniversaries[0].value.toString("dd.mm.yyyy");
        errors << QObject::tr("Anniversary saved as name day, contact %1").arg(item.visibleName);
    }
    // Addresses (only two for this format)
    PostalAddress homeAddr, workAddr;
    foreach(const PostalAddress& a, item.addrs) {
        if (a.types.contains("home", Qt::CaseInsensitive) && homeAddr.isEmpty())
            homeAddr = a;
        else if (a.types.contains("work", Qt::CaseInsensitive) && workAddr.isEmpty())
            workAddr = a;
        else // sad but true
            errors << S_ERR_EXTRA_TAG.arg(S_ADDR).arg(a.toString(true)).arg(item.visibleName);\
    }
    // Save home address
    row << homeAddr.street << homeAddr.postalCode << homeAddr.city << homeAddr.region << homeAddr.country;
    // Work data
    row << item.organization << item.title;
    if (!item.title.isEmpty())
        errors << QObject::tr("Job title saved as Department");
    // Save work address
    row << workAddr.street << workAddr.postalCode << workAddr.city << workAddr.region << workAddr.country;
    // Various phone types
    QString fax;
    QStringList homePhones, workPhones, cellPhones;
    foreach(const Phone& ph, item.phones) {
        if (ph.types.contains("fax", Qt::CaseInsensitive)) {
            if (fax.isEmpty())
                fax = ph.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_PHONE).arg(ph.value).arg(item.visibleName);
        }
        else if (ph.types.contains("home", Qt::CaseInsensitive)) {
            if (homePhones.count()<4)
                homePhones << ph.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_PHONE).arg(ph.value).arg(item.visibleName);
        }
        else if (ph.types.contains("work", Qt::CaseInsensitive)) {
            if (workPhones.count()<4)
                workPhones << ph.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_PHONE).arg(ph.value).arg(item.visibleName);
        }
        else if (ph.types.contains("cell", Qt::CaseInsensitive)) {
            if (cellPhones.count()<4)
                cellPhones << ph.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_PHONE).arg(ph.value).arg(item.visibleName);
        }
    }
    int i;
    for (i=homePhones.count(); i<4; i++) homePhones << "";
    for (i=workPhones.count(); i<4; i++) workPhones << "";
    for (i=cellPhones.count(); i<4; i++) cellPhones << "";
    row << fax;
    foreach(const QString& number, homePhones)
        row << number;
    foreach(const QString& number, workPhones)
        row << number;
    foreach(const QString& number, cellPhones)
        row << number;
    // Emails
    i=0;
    foreach(const Email& em, item.emails) {
        if (i<4)
            row << em.value;
        else
            errors << S_ERR_EXTRA_TAG.arg(S_EMAIL).arg(em.value).arg(item.visibleName);
        i++;
    }
    for (i=item.emails.count(); i<4; i++)
        row << "";
    // Web
    row << item.url;
    // TODO implement multiple urls for vCard 4.0, instead WWW2-WWW4
    QStringList addUrls = item.unknownTags.findByName("WWW");
    if (addUrls.count()>0)
        row << addUrls[0];
    else
        row << "";
    if (addUrls.count()>1)
        row << addUrls[1];
    else
        row << "";
    if (addUrls.count()>2)
        row << addUrls[2];
    else
        row << "";
    // IMs
    QMap<QString, QString> ims;
    foreach(const Messenger& im, item.ims) {
        if (im.types.contains("gadu-gadu", Qt::CaseInsensitive)) {
            if (!ims.contains("gadu-gadu"))
                ims["gadu-gadu"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("Yahoo", Qt::CaseInsensitive)) {
            if (!ims.contains("Yahoo"))
                ims["Yahoo"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("msn", Qt::CaseInsensitive)) {
            if (!ims.contains("msn"))
                ims["msn"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("icq", Qt::CaseInsensitive)) {
            if (!ims.contains("icq"))
                ims["icq"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("aim", Qt::CaseInsensitive)) {
            if (!ims.contains("aim"))
                ims["aim"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("xmpp", Qt::CaseInsensitive)) {
            if (!ims.contains("xmpp"))
                ims["xmpp"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("skype", Qt::CaseInsensitive)) {
            if (!ims.contains("skype"))
                ims["skype"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
        else if (im.types.contains("tlen", Qt::CaseInsensitive)) {
            if (!ims.contains("tlen"))
                ims["tlen"] = im.value;
            else
                errors << S_ERR_EXTRA_TAG.arg(S_IM).arg(im.value).arg(item.visibleName);
        }
    }
    QStringList blog = item.unknownTags.findByName("BLOG");
    if (!blog.isEmpty())
        row << blog[0];
    row << item.description;
    // Check other lost fields
    LOSS_DATA(S_PHOTO, !item.photo.isEmpty());
    return true;
}

bool OsmoProfile::present(const QStringList &row, const QString &colName)
{
    if (columnIndexes.contains(colName))
        return CSVProfileBase::present(row, columnIndexes[colName]);
    else
        return false;
}

QString OsmoProfile::value(const QStringList &row, const QString &colName)
{
    if (present(row, colName))
        return row[columnIndexes[colName]];
    else
        return "";
}

QStringList OsmoProfile::makeHeader()
{
    return QStringList()
        << "Group"
        << "First name" << "Last name" << "Second name"
        << "Nickname" << "Tags" << "Birthday date" << "Name day date"
        << "Home address" << "Home postcode" << "Home city" << "Home state" << "Home country"
        << "Organization" << "Department"
        << "Work address" << "Work postcode" << "Work city" << "Work state" << "Work country"
        << "Fax"
        << "Home phone" << "Home phone 2" << "Home phone 3" << "Home phone 4"
        << "Work phone" << "Work phone 2" << "Work phone 3" << "Work phone 4"
        << "Cell phone" << "Cell phone 2" << "Cell phone 3" << "Cell phone 4"
        << "E-Mail" << "E-Mail 2" << "E-Mail 3" << "E-Mail 4"
        << "WWW" << "WWW 2" << "WWW 3" << "WWW 4"
        << "IM Gadu-Gadu" << "IM Yahoo" << "IM MSN" << "IM ICQ" << "IM AOL"
        << "IM Jabber" << "IM Skype" << "IM Tlen" << "Blog" << "Additional info";
}
