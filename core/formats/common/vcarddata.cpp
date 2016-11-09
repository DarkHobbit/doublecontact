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

#include "globals.h"
#include "vcarddata.h"

#define MAX_NAMES 5
// TODO move MAX_NAMES to globals and use in contact dialog as high editors limit
#define MAX_BASE64_LEN 74
#define MAX_QUOTED_PRINTABLE_LEN 76

bool VCardData::importRecords(QStringList &lines, ContactList& list, bool append, QStringList& errors)
{
    bool recordOpened = false;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // non-standart types also may be non-latin
    QString defaultEmptyPhoneType =  Phone::standardTypes.unTranslate(gd.defaultEmptyPhoneType);
    ContactItem item;
    if (!append)
        list.clear();
    // Merge quoted-printable linesets
    for (int i=0; i<lines.count(); i++) {
        if (i>=lines.count()) break; // need, because count changed inside this cycle
        if (lines[i].contains("QUOTED-PRINTABLE", Qt::CaseInsensitive))
            while (lines[i].right(1)=="=" && i<lines.count()-1) {
                lines[i].remove(lines[i].length()-1, 1);
                lines[i] += lines[i+1];
                lines.removeAt(i+1);
            }
    }
    // Collect records
    for (int line=0; line<lines.count(); line++) {
        const QString& s = lines[line];
        if (s.isEmpty()) // vcf can contain empty lines
            continue;
        if (s.startsWith("BEGIN:VCARD", Qt::CaseInsensitive)) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
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
            encoding = "";
            charSet = "";
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
                    // TODO split such records as home,pref,cell in one type
                else if (vType[i].startsWith("VALUE=", Qt::CaseInsensitive))
                    // for PHOTO/URI, at least
                    typeVal = vType[i].mid(QString("VALUE=").length());
                else // "TYPE=" can be omitted in some addressbooks
                    types << codec->toUnicode(vType[i].toLocal8Bit());
            }
            if ((!types.isEmpty()) && (tag!="TEL") && (tag!="EMAIL") && (tag!="ADR") && (tag!="PHOTO"))
                errors << QObject::tr("Unexpected TYPE appearance at line %1: tag %2").arg(line+1).arg(tag);
            // TODO x-syncmlref save!!!
            // Known tags
            if (tag=="VERSION")
                item.version = decodeValue(vValue[0], errors);
            else if (tag=="FN")
                item.fullName = decodeValue(vValue[0], errors);
            else if (tag=="N") {
                foreach (const QString& name, vValue)
                    item.names << decodeValue(name, errors);
                // If empty parts not in-middle, remove it
                item.dropFinalEmptyNames();
            }
            else if (tag=="NOTE")
                item.description = decodeValue(vValue[0], errors);
            else if (tag=="SORT-STRING")
                item.sortString = decodeValue(vValue[0], errors);
            else if (tag=="TEL") {
                Phone phone;
                phone.number = decodeValue(vValue[0], errors);
                // Phone type(s)
                if (types.isEmpty()) {
                    QString surName = "";
                    if (!item.names.isEmpty())
                        surName = " (" + item.names[0] + ")"; // TODO m.b. use it in other messages AFTER NAME!!!
                    errors << QObject::tr("Missing phone type at line %1: %2%3").arg(line+1).arg(vValue[0]).arg(surName);
                    // TODO mb. no type is valid (in this case compare container and contact edit dialog must be updated)
                    phone.tTypes << defaultEmptyPhoneType;
                }
                else phone.tTypes = types;
                item.phones.push_back(phone);
            }
            else if (tag=="EMAIL") {
                // Some phones write empty EMAIL tag even if no email (i.e SE W300i in vCard 2.1)
                if (vValue[0].isEmpty())
                    continue;
                Email email;
                email.address = decodeValue(vValue[0], errors);
                if (types.isEmpty()) // maybe, it not a bug; some devices allows email without type
                    email.emTypes << "pref";
                else
                    email.emTypes = types;
                item.emails.push_back(email);
            }
            else if (tag=="BDAY")
                importDate(item.birthday, decodeValue(vValue[0], errors), errors);
            else if (tag=="X-ANNIVERSARY") {
                DateItem di;
                importDate(di, decodeValue(vValue[0], errors), errors);
                item.anniversaries.push_back(di);
            }
            else if (tag=="PHOTO") {
                if (typeVal.startsWith("URI", Qt::CaseInsensitive)) {
                    item.photoType = "URL";
                    item.photoUrl = decodeValue(vValue[0], errors);
                }
                else {
                    item.photoType = types[0];
                    if (item.photoType.toUpper()!="JPEG" && item.photoType.toUpper()!="PNG")
                        errors << QObject::tr("Unsupported photo type at line %1: %2").arg(line+1).arg(typeVal);
                    if (encoding=="B" || encoding=="BASE64") {
                        QString binaryData = vValue[0];
                        while (line<lines.count()-1 && !lines[line+1].trimmed().isEmpty() && lines[line+1].left(1)==" ") {
                            binaryData += lines[line+1];
                            line++;
                        }
                        if (lines[line+1].trimmed().isEmpty()) line++;
                        item.photo = QByteArray::fromBase64(binaryData.toLatin1());
                    }
                    else
                        errors << QObject::tr("Unknown encoding type at line %1: %2").arg(line+1).arg(encoding);
                }
            }
            else if (tag=="ORG")
                item.organization = decodeValue(vValue[0], errors);
            else if (tag=="TITLE")
                item.title = decodeValue(vValue[0], errors);
            else if (tag=="ADR") {
                if (types.contains("home", Qt::CaseInsensitive))
                    importAddress(item.addrHome, types, vValue, errors);
                else if (types.contains("work", Qt::CaseInsensitive))
                    importAddress(item.addrWork, types, vValue, errors);
                else
                    errors << QObject::tr("Unknown address type at line %1: %2").arg(line+1).arg(types.join(";"));
            }
            // Internet
            else if (tag=="NICKNAME")
                item.nickName = decodeValue(vValue[0], errors);
            else if (tag=="URL")
                item.url = decodeValue(vValue[0], errors);
            else if (tag=="X-JABBER")
                item.jabberName = decodeValue(vValue[0], errors);
            else if (tag=="X-ICQ")
                item.icqName = decodeValue(vValue[0], errors);
            else if (tag=="X-SKYPE-USERNAME")
                item.skypeName = decodeValue(vValue[0], errors);
            /*else if (tag=="IMPP") {
                // Second : in vValue[0]; vType also contains X-SYNCMLREF
                // TODO implement in vCard 4.0 as alternative

            }*/
            // Identifier
            else if (tag=="X-IRMC-LUID")
                item.id = decodeValue(vValue[0], errors);
            // Known but un-editing tags
            else if (
                tag=="LABEL"
                || tag=="CATEGORIES"// MyPhoneExplorer YES, embedded android export NO
                || tag=="X-ACCOUNT" // MyPhoneExplorer YES, embedded android export NO
            )
            { // TODO other from rfc 2426
                item.otherTags.push_back(TagValue(vType.join(";"),
                    decodeValue(vValue.join(";"), errors)));
            }            
            // Unknown tags
            else {
                item.unknownTags.push_back(TagValue(vType.join(";"),
                    decodeValue(vValue.join(";"), errors)));
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
    foreach (const ContactItem& item, list)
        exportRecord(lines, item);
    return (!list.isEmpty());
}

void VCardData::exportRecord(QStringList &lines, const ContactItem &item)
{
    // Format version
    formatVersion = gd.preferredVCFVersion;
    if (gd.useOriginalFileVersion && (item.originalFormat=="VCARD")) {
        if (item.version=="2.1")
            formatVersion = GlobalConfig::VCF21;
        else if (item.version=="3.0")
            formatVersion = GlobalConfig::VCF30;
        // TODO VCF40
    }
    // Encoding/charSet prefix
    charSet = "UTF-8"; // TODO save original charset in ContactItem
    encoding = formatVersion==GlobalConfig::VCF21 ? "QUOTED-PRINTABLE" : "";
    // Header
    lines << "BEGIN:VCARD";
    lines << QString("VERSION:") + (formatVersion==GlobalConfig::VCF21 ? "2.1" : "3.0");
    // Known tags
    if (!item.names.isEmpty()) {
        QString seps = "";
        if (item.names.count()<MAX_NAMES && formatVersion!=GlobalConfig::VCF21)
            seps.fill(';', MAX_NAMES-item.names.count());
        lines << encodeAll("N", 0, false, item.names.join(";")) + seps;
    }
    if (!item.fullName.isEmpty())
        lines << encodeAll("FN", 0, false, item.fullName);
    if (!item.sortString.isEmpty())
        lines << encodeAll("SORT-STRING", 0, false, item.sortString);
    if (!item.nickName.isEmpty())
        lines << encodeAll("NICKNAME", 0, false, item.nickName);
    foreach (const Phone& ph, item.phones)
        lines << (QString("TEL") + encodeTypes(ph.tTypes)+":"+ph.number);
    foreach (const Email& em, item.emails)
        lines << QString("EMAIL") + encodeTypes(em.emTypes)+":"+em.address;
    /*
    // for Sony Ericsson devices TODO to settings (emulate, fake...)
    if (item.emails.isEmpty())
        lines << QString("EMAIL;INTERNET;PREF:");
    if (item.emails.count()<2)
        lines << QString("EMAIL;INTERNET:");
    if (item.emails.count()<3)
        lines << QString("EMAIL;INTERNET:");
    */
    if (!item.birthday.isEmpty())
        lines << QString("BDAY:") + exportDate(item.birthday);
    foreach (const DateItem& ann, item.anniversaries)
        lines << QString("X-ANNIVERSARY:") + exportDate(ann);
    // Organization, addresses
    if (!item.addrHome.isEmpty())
        lines << exportAddress(item.addrHome);
    if (!item.addrWork.isEmpty())
        lines << exportAddress(item.addrWork);
    if (!item.organization.isEmpty())
        lines << encodeAll("ORG", 0, true, item.organization);
    if (!item.title.isEmpty())
        lines << encodeAll("TITLE", 0, true, item.title);
    // Internet 1
    if (!item.url.isEmpty())
        lines << encodeAll("URL", 0, false, item.url);
    // Photos
    if (item.photoType=="URL")
        lines << QString("PHOTO;VALUE=uri:") + item.photoUrl;
    else if (!item.photoType.isEmpty()) {
        QString base64Line = QString("PHOTO;ENCODING=B;TYPE=") + item.photoType + ":" + item.photo.toBase64();
        while (base64Line.length()>MAX_BASE64_LEN) {
            lines << base64Line.left(MAX_BASE64_LEN);
            base64Line.remove(0, MAX_BASE64_LEN);
            base64Line = QString(" ") + base64Line;
        }
        if (!base64Line.isEmpty())
            lines << base64Line;
        lines << "";
    }
    if (!item.description.isEmpty())
        lines << encodeAll("NOTE", 0, true, item.description);
    // Internet 2
    // TODO use IMPP instead this, if vcard4 profile selected
    if (!item.jabberName.isEmpty())
        lines << encodeAll("X-JABBER", 0, false, item.jabberName);
    if (!item.icqName.isEmpty())
        lines << encodeAll("X-ICQ", 0, false, item.icqName);
    if (!item.skypeName.isEmpty())
        lines << encodeAll("X-SKYPE-USERNAME", 0, false, item.skypeName);
    // Identifier
    // TODO need support for other identifier types (apple?) and more strong detection
    if (!item.id.isEmpty() && item.id.length()>=10) // second condition separate from other ID kinds. TODO: need more strong crit.
        lines << QString("X-IRMC-LUID:") + encodeValue(item.id, QString("X-IRMC-LUID:").length());
    // Known but un-editing tags
    foreach (const TagValue& tv, item.otherTags)
            lines << QString(tv.tag + ":" + tv.value);
    // Unknown tags
    foreach (const TagValue& tv, item.unknownTags)
            lines << QString(tv.tag + ":" + tv.value);
    lines << "END:VCARD";
}

QString VCardData::decodeValue(const QString &src, QStringList& errors) const
{
    QTextCodec *codec; // for values
    // Charset
    if (charSet.isEmpty())
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
                if (i<src.length()-1)
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

// TODO Maybe, move it into DateItem::fromString
void VCardData::importDate(DateItem &item, const QString &src, QStringList& errors) const
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

void VCardData::importAddress(PostalAddress &item, const QStringList& aTypes, const QStringList& values, QStringList &errors) const
{
    item.clear();
    item.paTypes = aTypes;
    if (values.count()>0) item.offBox = decodeValue(values[0], errors);
    if (values.count()>1) item.extended = decodeValue(values[1], errors);
    if (values.count()>2) item.street = decodeValue(values[2], errors);
    if (values.count()>3) item.city = decodeValue(values[3], errors);
    if (values.count()>4) item.region = decodeValue(values[4], errors);
    if (values.count()>5) item.postalCode = decodeValue(values[5], errors);
    if (values.count()>6) item.country = decodeValue(values[6], errors);
}

void VCardData::checkQPSoftBreak(QString& buf, QString& lBuf, int prefixLen, int addSize, bool lastChar) const
{
    // If port to C++11, it must be lambda...
    // Rule 5 (RFC 2045). Soft Line Breaks
    int limit = MAX_QUOTED_PRINTABLE_LEN;
    if (buf.isEmpty()) // first sub-line
        limit -= prefixLen;
    if (!lastChar)
        limit--;
    if (lBuf.length()>limit-addSize) {
        if (!buf.isEmpty())
            buf += "=\x0d\n";
        buf += lBuf;
        lBuf.clear();
    }
}

QString VCardData::encodeValue(const QString &src, int prefixLen) const
{
    QTextCodec *codec;
    // Charset
    if (charSet.isEmpty())
        codec = QTextCodec::codecForName("UTF-8");
    else
        codec = QTextCodec::codecForName(charSet.toLocal8Bit());
    if (encoding.toUpper()=="QUOTED-PRINTABLE") {
        // We can't apply codec->fromUnicode to entire string, because
        // we must find ascii-able characters, but variable character length
        // may cause false match. We must check EACH character.
        QString buf, lBuf;
        for (int i=0; i<src.count(); i++) {
            QChar ch = src[i];
            QByteArray bytes = codec->fromUnicode(ch);
            // Can we use Literal representation?
            // Rule 2 (RFC 2045). Literal representation
            bool useLiteral = (ch>=33 && ch<=126 && ch!=61);
            // Rule 3. Spaces
            if (ch==0x20 || ch==0x09)
                useLiteral = i<src.count()-1;
            // Represent!
            bool lastChar = i==src.count()-1;
            if (useLiteral) {
                checkQPSoftBreak(buf, lBuf, prefixLen, 1, lastChar);
                lBuf += ch;
            }
            else { // Rule 1. General 8bit representation
                for(int j=0; j<bytes.count(); j++) {
                    checkQPSoftBreak(buf, lBuf, prefixLen, 3, lastChar && j==bytes.count()-1);
                    QString hex = QString::number((uchar)bytes[j], 16).toUpper();
                    if (hex.length()==1)
                        hex = QString("0")+hex;
                    lBuf += "=" + hex;
                }
            }
        }
        if (!lBuf.isEmpty()) {
            if (!buf.isEmpty())
                buf += "=\x0d\n";
            buf += lBuf;
        }
        // Rule 4. Line Breaks - apply in caller
        return buf;
    }
    else
        return QString::fromLocal8Bit(codec->fromUnicode(src));
}

QString VCardData::encodeAll(const QString &tag, const QStringList *aTypes, bool forceCharSet, const QString &value) const
{
    QString encStr = tag;
    if (aTypes)
        encStr += encodeTypes(*aTypes);
    // Encoding and charset info
    if (charSet!="UTF-8" || !encoding.isEmpty() || (forceCharSet && value.toLatin1()!=value)) {
        encStr += ";CHARSET=" + charSet;
        if (!encoding.isEmpty())
            encStr += ";ENCODING=" + encoding;
    }
    encStr += ":";
    QString valStr = encodeValue(value, encStr.length());
    // Optimize ecoding :)
    if (!valStr.contains("=") && charSet=="UTF-8" && encoding=="QUOTED-PRINTABLE")
        encStr = tag + ":";
    return encStr + valStr;
}

QString VCardData::encodeTypes(const QStringList &aTypes) const
{
    QString typeStr = ";";
    if (formatVersion!=GlobalConfig::VCF21)
        typeStr += "TYPE=";
    // typeStr += aTypes.join(","); // value list
    typeStr += aTypes.join(formatVersion==GlobalConfig::VCF21 ? ";" : ";TYPE="); // parameter list; RFC 2426 allows both form
    return encodeValue(typeStr, 0);
}

QString VCardData::exportDate(const DateItem &item) const
{
    return
        formatVersion==GlobalConfig::VCF21 ?
                item.toString(DateItem::ISOBasic) : item.toString(DateItem::ISOExtended);
}

QString VCardData::exportAddress(const PostalAddress &item) const
{
    return encodeAll("ADR", &item.paTypes, true,
                item.offBox + ";" + item.extended
        + ";" + item.street + ";" + item.city + ";" + item.region
        + ";" + item.postalCode + ";" + item.country);
}

