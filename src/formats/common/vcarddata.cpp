/* Double Contact
 *
 * Module: VCard data export/import (both for file and network media)
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QByteArray>
#include <QObject>
#include <QTextCodec>

#include "vcarddata.h"

bool VCardData::importRecords(QStringList &lines, ContactList& list, bool append, QStringList& errors)
{
    bool recordOpened = false;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // non-standart types also may be non-latin
    ContactItem item;
    if (!append)
        list.clear();
    // Merge quoted-printable linesets
    if (lines.count()>1)
    for (int i=lines.count()-2;i>=0;i--) {
        if (lines[i].right(1)=="=" && lines[i+1].left(1)=="=") {
            lines[i] += lines[i+1].mid(1);
            lines.removeAt(i+1);
        }
    }
    // Collect records
    for (int line=0; line<lines.count(); line++) {
        const QString& s = lines[line];
        if (s.startsWith("BEGIN:VCARD", Qt::CaseInsensitive)) {
            recordOpened = true;
            item.clear();
            item.originalFormat = "VCARD";
        }
        else if (s.startsWith("END:VCARD", Qt::CaseInsensitive)) {
            recordOpened = false;
            item.calculateFields();
            list.push_back(item);
        }
        else {
            // Split type:value
            int scPos = s.indexOf(":");
            if (scPos==-1) {
                item.unknownTags.push_back(TagValue(s, ""));
                continue;
            }
            QStringList vType = s.left(scPos).split(";");
            QStringList vValue = s.mid(scPos+1).split(";");
            const QString tag = vType[0].toUpper();
            // Encoding, charset, types
            QString encoding = "";
            QString charSet = "";
            QString typeVal = ""; // for PHOTO/URI, at least
            QStringList types;
            for (int i=1; i<vType.count(); i++) {
                if (vType[i].startsWith("ENCODING=", Qt::CaseInsensitive))
                    encoding = vType[i].mid(QString("ENCODING=").length());
                else if (vType[i].startsWith("CHARSET=", Qt::CaseInsensitive))
                    charSet = vType[i].mid(QString("CHARSET=").length());
                else if (vType[i].startsWith("TYPE=", Qt::CaseInsensitive))
                    // non-standart types may be non-latin
                    types << codec->toUnicode(vType[i].mid(QString("TYPE=").length()).toLocal8Bit());
                else if (vType[i].startsWith("VALUE=", Qt::CaseInsensitive))
                    // for PHOTO/URI, at least
                    typeVal = vType[i].mid(QString("VALUE=").length());
                else
                    item.unknownTags.push_back(TagValue(vType.join(";"), vValue.join(";"))); // TODO partiallyKnownTag?
            }
            if ((!types.isEmpty()) && (tag!="TEL") && (tag!="EMAIL") && (tag!="ADR") && (tag!="PHOTO"))
                errors << QObject::tr("Unexpected TYPE appearance at line %1").arg(line+1);
            // Known tags
            if (tag=="VERSION")
                item.version = decodeValue(vValue[0], encoding, charSet, errors);
            else if (tag=="FN")
                item.fullName = decodeValue(vValue[0], encoding, charSet, errors);
            else if (tag=="N")
                foreach (const QString& name, vValue)
                    item.names << decodeValue(name, encoding, charSet, errors);
            else if (tag=="NOTE")
                item.description = decodeValue(vValue[0], encoding, charSet, errors);
            else if (tag=="TEL") {
                Phone phone;
                phone.number = decodeValue(vValue[0], encoding, charSet, errors);
                // Phone type(s)
                if (types.isEmpty()) {
                    errors << QObject::tr("Missing phone type at line %1").arg(line+1);
                    phone.tTypes << "pref";
                }
                phone.tTypes = types;
                item.phones.push_back(phone);
            }
            else if (tag=="EMAIL") {
                Email email;
                email.address = decodeValue(vValue[0], encoding, charSet, errors);
                if (types.isEmpty()) // maybe, it not a bug; some devices allows email without type
                    email.emTypes << "pref";
                else
                    email.emTypes = types;
                item.emails.push_back(email);
            }
            else if (tag=="BDAY")
                importDate(item.birthday, decodeValue(vValue[0], encoding, charSet, errors), errors);
            else if (tag=="X-ANNIVERSARY") {
                DateItem di;
                importDate(di, decodeValue(vValue[0], encoding, charSet, errors), errors);
                item.anniversaries.push_back(di);
            }
            else if (tag=="PHOTO") {
                if (typeVal.startsWith("URI", Qt::CaseInsensitive)) {
                    item.photoType = "URL";
                    item.photoUrl = decodeValue(vValue[0], encoding, charSet, errors);
                }
                else {
                    item.photoType = types[0];
                    if (item.photoType.toUpper()!="JPEG")
                        errors << QObject::tr("Unsupported photo type at line %1: ").arg(line+1)+typeVal;
                    if (encoding=="B") {
                        QString binaryData = vValue[0];
                        while (line<lines.count() && !lines[line+1].trimmed().isEmpty()) {
                            binaryData += lines[line+1];
                            line++;
                        }
                        if (lines[line+1].trimmed().isEmpty()) line++;
                        item.photo = QByteArray::fromBase64(binaryData.toLatin1());
                    }
                    else
                        errors << QObject::tr("Unknown type at line %1").arg(line+1);
                }
            }
            // TODO ADR, ORG...

            // Known but un-editing tags
            else if (tag=="LABEL") { // TODO other from rfc 2426
                item.otherTags.push_back(TagValue(vType.join(";"),
                    decodeValue(vValue.join(";"), encoding, charSet, errors)));
            }
            // Unknown tags
            else {
                item.unknownTags.push_back(TagValue(vType.join(";"),
                    decodeValue(vValue.join(";"), encoding, charSet, errors)));
            }
        }

    }
    if (recordOpened) {
        item.calculateFields();
        list.push_back(item);
        errors << QObject::tr("Last section not closed");
    }
    // Unknown tags statistics
    int totalUnknownTags = 0;
    foreach (const ContactItem& _item, list)
        totalUnknownTags += _item.unknownTags.count();
    if (totalUnknownTags)
        errors << QObject::tr("%1 unknown tags found").arg(totalUnknownTags);
    // Ready
    return (!list.isEmpty());
}

bool VCardData::exportRecords(QStringList &lines, const ContactList &list)
{
    // TODO
}

QString VCardData::decodeValue(const QString &src, const QString &encoding, const QString &charSet, QStringList& errors)
{
    QTextCodec *codec; // for values
    // Charset
    if (charSet.isEmpty()) // TODO not tested on Linux
        codec = QTextCodec::codecForName("UTF-8");
    else
        codec = QTextCodec::codecForName(charSet.toLocal8Bit());
    if (!codec) {
        errors << QObject::tr("Unknown charset: ")+charSet;
        return "";
    }
    // Encoding
    if (encoding.isEmpty())
        return codec->toUnicode(src.toLocal8Bit());
    else if (encoding.toUpper()=="QUOTED-PRINTABLE") {
        QByteArray res;
        bool ok;
        for (int i=0; i<src.length(); i++) {
            if (src[i]=='=') {
                if (src[i+1]==' ') i++; // sometime bad space after = appears
                const quint8 code = src.mid(i+1, 2).toInt(&ok, 16);
                res.append(code);
                i += 2;
            }
            else
                res += src[i];
        }
        return codec->toUnicode(res);
    }
    else {
        errors << QObject::tr("Unknown encoding: ")+encoding;
        return "";
    }
}

void VCardData::importDate(DateItem &item, const QString &src, QStringList& errors)
{
    int tPos = src.indexOf("T", 0, Qt::CaseInsensitive);
    item.hasTime = (tPos!=-1);
    if (item.hasTime) {
        int sPos = src.mid(tPos+1).indexOf("-");
        int zPos = src.mid(tPos+1).indexOf("Z", 0, Qt::CaseInsensitive);
        item.hasTimeZone = (sPos!=-1) || (zPos!=-1);
        if (sPos!=-1) { // 1987-09-27T08:30:00-06:00
            item.value = QDateTime::fromString(
                src.left(tPos+sPos+1), "yyyy-MM-ddTHH:mm:ss");
            QString zone = src.mid(tPos+tPos+1);
            bool ok;
            item.zoneHour = zone.left(zone.indexOf(":")).toShort(&ok);
            if (ok)
                item.zoneMin = zone.right(zone.indexOf(":")).toShort(&ok);
            if (!ok)
                errors << QObject::tr("Invalid timezone: ") + src;
        }
        else if (zPos!=-1) { // 1953-10-15T23:10:00Z
            item.value = QDateTime::fromString(
                src.left(tPos+zPos+1), "yyyy-MM-ddTHH:mm:ss");
            item.zoneHour = 0;
            item.zoneMin = 0;
        }
        else
            item.value = QDateTime::fromString(
                src, "yyyy-MM-ddTHH:mm:ss");
    }
    else {
        item.value = QDateTime::fromString(src, "yyyy-MM-dd");
        if (!item.value.isValid())
            // try non-standard format from some old Nokias
            item.value = QDateTime::fromString(src, "yyyyMMdd");
    }
    if (!item.value.isValid())
        errors << QObject::tr("Invalid datetime: ") + src;
}
