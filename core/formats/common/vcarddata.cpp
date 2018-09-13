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

#define MAX_BASE64_LEN 74
#define MAX_QUOTED_PRINTABLE_LEN 76

VCardData::VCardData()
{
    useOriginalFileVersion = gd.useOriginalFileVersion;
    skipEncoding = false;
    skipDecoding = false;
    formatVersion = GlobalConfig::VCF30;
    _forceVersion = false;
}

void VCardData::forceVersion(GlobalConfig::VCFVersion version)
{
    _forceVersion = true;
    formatVersion = version;
}

void VCardData::unforceVersion()
{
    _forceVersion = false;
}

void VCardData::setSkipCoding(bool _skipEncoding, bool _skipDecoding)
{
    skipEncoding = _skipEncoding;
    skipDecoding = _skipDecoding;
}

bool VCardData::importRecords(QStringList &lines, ContactList& list, bool append, QStringList& errors)
{
    bool recordOpened = false;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // non-standart types also may be non-latin
    QString defaultEmptyPhoneType =  Phone::standardTypes.unTranslate(gd.defaultEmptyPhoneType);
    ContactItem item;
    if (!append)
        list.clear();
    QString visName = "";
    // Merge quoted-printable linesets
    for (int i=0; i<lines.count(); i++) {
        if (i>=lines.count()) break; // need, because count changed inside this cycle
        if (lines[i].contains("QUOTED-PRINTABLE", Qt::CaseInsensitive))
            while (lines[i].right(1)=="=" && i<lines.count()-1) {
                lines[i].remove(lines[i].length()-1, 1);
                if (lines[i+1].left(1)=="\t") // Folding by tab, for example in Mozilla Thunderbird VCFs
                    lines[i+1].remove(0, 1);
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
            visName.clear();
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
            int syncMLRef = -1;
            for (int i=1; i<vType.count(); i++) {
                if (vType[i].startsWith("ENCODING=", Qt::CaseInsensitive))
                    encoding = vType[i].mid(QString("ENCODING=").length()).toUpper();
                else if (vType[i].startsWith("CHARSET=", Qt::CaseInsensitive))
                    charSet = vType[i].mid(QString("CHARSET=").length());
                else if (vType[i].startsWith("TYPE=", Qt::CaseInsensitive)
                         || vType[i].startsWith("LABEL=", Qt::CaseInsensitive)) {// TODO see vCard 4.0, m.b. LABEL= points to non-standard?
                    // non-standart types may be non-latin
                    QString typeCand = vType[i];
                    typeCand.remove("TYPE=").remove("LABEL=");
                    if (!skipDecoding)
                        typeCand = codec->toUnicode(typeCand.toLocal8Bit());
                    // Detect and split types, composed as value list (RFC)
                    if (typeCand.contains(",")) {
                        QStringList typesAsValueList = typeCand.split(",");
                        foreach (const QString& vlType, typesAsValueList)
                            types << vlType;
                    }
                    else // one value - it's more fast in most cases
                        types << typeCand;
                }
                else if (vType[i].startsWith("VALUE=", Qt::CaseInsensitive))
                    // for PHOTO/URI, at least
                    typeVal = vType[i].mid(QString("VALUE=").length());
                else if (vType[i].startsWith("X-SYNCMLREF", Qt::CaseInsensitive))
                    syncMLRef = vType[i].mid(QString("X-SYNCMLREF").length()).toInt();
                else {
                    // "TYPE=" can be omitted in some addressbooks
                    // But it also may be encoding (~~)
                    if (vType[i].startsWith("QUOTED-PRINTABLE", Qt::CaseInsensitive)
                            || vType[i].startsWith("BASE64", Qt::CaseInsensitive))
                        encoding = vType[i];
                    else {// type, type...
                        if (skipDecoding)
                            types << vType[i];
                        else
                            types << codec->toUnicode(vType[i].toLocal8Bit());
                    }
                }
            }
            if ((!types.isEmpty()) && (tag!="TEL")
                    && (tag!="EMAIL") && (tag!="ADR") && (tag!="PHOTO") && (tag!="IMPP"))
                errors << QObject::tr("Unexpected TYPE appearance at line %1: tag %2").arg(line+1).arg(tag);
            // Known tags
            if (tag=="VERSION")
                item.version = decodeValue(vValue[0], errors);
            else if (tag=="FN") {
                item.fullName = decodeValue(vValue[0], errors);
                // Name compilation for error messages
                if (visName.isEmpty() && !item.fullName.isEmpty())
                    visName = " (" + item.fullName + ")";
            }
            else if (tag=="N") {
                foreach (const QString& name, vValue)
                    item.names << decodeValue(name, errors);
                // If empty parts not in-middle, remove it
                item.dropFinalEmptyNames();
                // Name compilation for error messages
                if (visName.isEmpty() && !item.names.isEmpty())
                    visName = " (" + item.formatNames() + ")";
            }
            else if (tag=="NOTE")
                item.description = decodeValue(vValue[0], errors);
            else if (tag=="SORT-STRING")
                item.sortString = decodeValue(vValue[0], errors);
            else if (tag=="TEL") {
                Phone phone;
                phone.value = decodeValue(vValue[0], errors);
                // Phone type(s)
                if (types.isEmpty()) {
                    errors << QObject::tr("Missing phone type at line %1: %2%3").arg(line+1).arg(vValue[0]).arg(visName);
                    // TODO mb. no type is valid (in this case compare container and contact edit dialog must be updated)
                    // TODO in this case make warning optional in settings (and, probably, false by default)
                    phone.types << defaultEmptyPhoneType.toUpper();
                }
                else phone.types = types;
                if (gd.warnOnNonStandardTypes)
                    foreach(const QString& tType, types) {
                        bool isStandard;
                        Phone::standardTypes.translate(tType, &isStandard);
                        if (!isStandard)
                            errors << QObject::tr("Non-standard phone type at line %1: %2%3").arg(line+1).arg(tType).arg(visName);
                    }
                phone.syncMLRef = syncMLRef;
                item.phones << phone;
            }
            else if (tag=="EMAIL") {
                // Some phones write empty EMAIL tag even if no email (i.e SE W300i in vCard 2.1)
                if (vValue[0].isEmpty())
                    continue;
                Email email;
                email.value = decodeValue(vValue[0], errors);
                if (types.isEmpty()) // maybe, it not a bug; some devices allows email without type
                    email.types << "pref";
                else
                    email.types = types;
                email.syncMLRef = syncMLRef;
                item.emails << email;
            }
            else if (tag=="BDAY")
                importDate(item.birthday, decodeValue(vValue[0], errors), errors);
            else if ((tag=="ANNIVERSARY") || (tag=="X-ANNIVERSARY"))
                importDate(item.anniversary, decodeValue(vValue[0], errors), errors);
            else if (tag=="PHOTO") {
                if (typeVal.startsWith("URI", Qt::CaseInsensitive)) {
                    item.photo.pType = "URL";
                    item.photo.url = decodeValue(vValue[0], errors);
                }
                else {
                    item.photo.pType = types[0];
                    if (item.photo.pType.toUpper()!="JPEG" && item.photo.pType.toUpper()!="PNG")
                        errors << QObject::tr("Unsupported photo type at line %1: %2%3").arg(line+1).arg(typeVal).arg(visName);
                    if (encoding=="B" || encoding=="BASE64") {
                        QString binaryData = vValue[0];
                        while (line<lines.count()-1 && !lines[line+1].trimmed().isEmpty() && lines[line+1].left(1)==" ") {
                            binaryData += lines[line+1];
                            line++;
                        }
                        if (lines[line+1].trimmed().isEmpty()) line++;
                        item.photo.data = QByteArray::fromBase64(binaryData.toLatin1());
                    }
                    else
                        errors << QObject::tr("Unknown encoding type at line %1: %2%3").arg(line+1).arg(encoding).arg(visName);
                }
            }
            else if (tag=="CATEGORIES" || tag=="X-CATEGORIES") // X- - some Nokia Suite versions
                foreach(const QString& val, vValue)
                    item.groups << decodeValue(val, errors);
            else if (tag=="X-NOKIA-PND-GROUP") { // Nokia NBF
                // For some groups per contact, each group wrote in separate X-NOKIA-PND-GROUP tag
                QTextCodec* utf16 = QTextCodec::codecForName("UTF-16");
                item.groups << utf16->toUnicode(vValue[0].toLocal8Bit());
            }
            else if (tag=="ORG")
                item.organization = decodeValue(vValue[0], errors);
            else if (tag=="TITLE")
                item.title = decodeValue(vValue[0], errors);
            else if (tag=="ADR") {
                PostalAddress addr;
                importAddress(addr, types, vValue, errors);
                if (types.isEmpty())
                    addr.types << "work";
                else
                    addr.types = types;
                addr.syncMLRef = syncMLRef;
                item.addrs << addr;
            }
            // Internet
            else if (tag=="NICKNAME")
                item.nickName = decodeValue(vValue[0], errors);
            else if (tag=="URL")
                item.url = decodeValue(vValue[0], errors);
            else if (tag=="X-JABBER") // Pre-vCard 4.0 non-standard IM tags
                item.ims << Messenger(vValue[0], "xmpp");
            else if (tag=="X-ICQ")
                item.ims << Messenger(vValue[0], "icq");
            else if (tag=="X-SKYPE-USERNAME")
                item.ims << Messenger(vValue[0], "skype");
            else if (tag=="IMPP") { // vCard 4.0
                Messenger im;
                im.value = decodeValue(vValue[0], errors);
                if (types.isEmpty())
                    im.types << "pref";
                else
                    im.types = types;
                im.syncMLRef = syncMLRef;
                item.ims << im;

            }
            // TODO nickname and url also can require x-syncmlref
            // Identifier
            else if (tag=="X-IRMC-LUID" || tag=="UID") {
                item.id = decodeValue(vValue[0], errors);
                item.idType = tag;
            }
            // Known but un-editing tags
            else if (
                tag=="LABEL"
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

bool VCardData::exportRecords(QStringList &lines, const ContactList &list, QStringList& errors)
{
    foreach (const ContactItem& item, list)
        exportRecord(lines, item, errors);
    return (!list.isEmpty());
}

void VCardData::exportRecord(QStringList &lines, const ContactItem &item, QStringList& errors)
{
    // Format version
    if (!_forceVersion) {
        formatVersion = gd.preferredVCFVersion;
        if (useOriginalFileVersion && (item.originalFormat=="VCARD")) {
            if (item.version=="2.1")
                formatVersion = GlobalConfig::VCF21;
            else if (item.version=="3.0")
                formatVersion = GlobalConfig::VCF30;
            else
                formatVersion = GlobalConfig::VCF40;
        }
    }
    // Encoding/charSet prefix
    charSet = "UTF-8"; // TODO save original charset in ContactItem
    encoding = formatVersion==GlobalConfig::VCF21 ? "QUOTED-PRINTABLE" : "";
    // Header
    lines << "BEGIN:VCARD";
    QString sVersion;
    switch (formatVersion) {
    case GlobalConfig::VCF21:
        sVersion = "2.1";
        break;
    case GlobalConfig::VCF30:
        sVersion = "3.0";
        break;
    default:
        sVersion = "4.0";
    }
    lines << QString("VERSION:") + sVersion;
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
        lines << (QString("TEL") + encodeTypes(ph.types, &Phone::standardTypes, ph.syncMLRef)+":"+ph.value);
    foreach (const Email& em, item.emails)
        lines << QString("EMAIL") + encodeTypes(em.types, &Email::standardTypes, em.syncMLRef)+":"+em.value;
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
    if (!item.anniversary.isEmpty()) {
        if (formatVersion>=GlobalConfig::VCF40)
            lines << QString("ANNIVERSARY:") + exportDate(item.anniversary);
        else
            lines << QString("X-ANNIVERSARY:") + exportDate(item.anniversary);
    }
    if (!item.groups.isEmpty()) {
        QString tagName = (formatVersion>=GlobalConfig::VCF40) ? "CATEGORIES" : "X-CATEGORIES";
        lines << encodeAll(tagName, 0, false, item.groups.join(";"));
    }
    // Organization, addresses
    foreach (const PostalAddress& addr, item.addrs)
        lines << exportAddress(addr);
    if (!item.organization.isEmpty())
        lines << encodeAll("ORG", 0, true, item.organization);
    if (!item.title.isEmpty())
        lines << encodeAll("TITLE", 0, true, item.title);
    // Internet 1
    if (!item.url.isEmpty())
        lines << encodeAll("URL", 0, false, item.url);
    // Photos
    if (item.photo.pType=="URL")
        lines << QString("PHOTO;VALUE=uri:") + item.photo.url;
    else if (!item.photo.pType.isEmpty()) {
        QString base64Line = QString("PHOTO;ENCODING=B;TYPE=") + item.photo.pType + ":" + item.photo.data.toBase64();
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
    foreach (const Messenger& im, item.ims) {
        // Use IMPP only if vcard4 profile selected
        // RFC 4770 defines IMPP for vcard3, but some devices with vcard3 don't know it
        // TODO maybe add additional restrictions for Ancient Android 2, Soneric, iPhone, etc.
        if ((formatVersion>=GlobalConfig::VCF40))
            lines << QString("IMPP") + encodeTypes(im.types, &Messenger::standardTypes, im.syncMLRef)+":"+im.value;
        else {
            if (im.types.contains("xmpp", Qt::CaseInsensitive))
                lines << encodeAll("X-JABBER", 0, false, im.value);
            else if (im.types.contains("icq", Qt::CaseInsensitive))
                lines << encodeAll("X-ICQ", 0, false, im.value);
            else if (im.types.contains("skype", Qt::CaseInsensitive))
                lines << encodeAll("X-SKYPE-USERNAME", 0, false, im.value);
            else if (!im.types.isEmpty())
                lines << encodeAll("X-" + im.types.join("+").toUpper(), 0, false, im.value);
            else
                errors << S_ERR_UNSUPPORTED_TAG.arg(item.visibleName).arg(S_IM);
        }
    }
    // Identifier
    // TODO need support for other identifier types (apple?) and more strong detection
    if (!item.id.isEmpty() && item.id.length()>=10 && item.idType!="Sequence") // second condition separate from other ID kinds. TODO: need more strong crit.
        lines << item.idType + ":" + encodeValue(item.id, QString(item.idType + ":").length());
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
    if (skipDecoding)
        return src;
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
    if (encoding.isEmpty() || encoding.startsWith("8BIT", Qt::CaseInsensitive))
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
        int sdPos = src.left(tPos).indexOf("-"); // - in long date format
        int stPos = src.mid(tPos+1).indexOf("-"); // - in timezone
        int zPos = src.mid(tPos+1).indexOf("Z", 0, Qt::CaseInsensitive); // timezone
        item.hasTimeZone = (stPos!=-1) || (zPos!=-1);
        if (stPos!=-1) { // 1987-09-27T08:30:00-06:00 or 19870927T083000-0600
            if (sdPos!=-1)
                item.value = QDateTime::fromString(
                    src.left(tPos+stPos+1), "yyyy-MM-ddTHH:mm:ss");
            else
                item.value = QDateTime::fromString(
                    src.left(tPos+stPos+1), "yyyyMMddTHHmmss");
            QString zone = src.mid(tPos+tPos+1);
            bool ok;
            item.zoneHour = zone.left(2).toShort(&ok);
            if (ok)
                item.zoneMin = zone.right(2).toShort(&ok);
            if (!ok)
                errors << QObject::tr("Invalid timezone: ") + src;
        }
        else if (zPos!=-1) { // 1953-10-15T23:10:00Z or 19531015T231000Z
            if (sdPos!=-1)
                item.value = QDateTime::fromString(
                    src.left(tPos+zPos+1), "yyyy-MM-ddTHH:mm:ss");
            else
                item.value = QDateTime::fromString(
                    src.left(tPos+zPos+1), "yyyyMMddTHHmmss");
            item.zoneHour = 0;
            item.zoneMin = 0;
        }
        else {
            if (sdPos!=-1)
                item.value = QDateTime::fromString(
                    src, "yyyy-MM-ddTHH:mm:ss");
            else
                item.value = QDateTime::fromString(
                    src, "yyyyMMddTHHmmss");
        }
    }
    else { // without time
        // TODO implement date without year: --0415 according vCard 4.0. Need flag or incorrect year
        int sdPos = src.indexOf("-"); // - in long date format
        if (sdPos!=-1)
            item.value = QDateTime::fromString(src, "yyyy-MM-dd");
        else
            item.value = QDateTime::fromString(src, "yyyyMMdd");
    }
    if (!item.value.isValid())
        errors << QObject::tr("Invalid datetime: ") + src;
}

void VCardData::importAddress(PostalAddress &item, const QStringList& aTypes, const QStringList& values, QStringList &errors) const
{
    item.clear();
    item.types = aTypes;
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
    else if (!skipEncoding)
        return QString::fromLocal8Bit(codec->fromUnicode(src));
    else
        return src;
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

QString VCardData::encodeTypes(const QStringList &aTypes, StandardTypes* st, int /*syncMLRef*/) const
{
    bool shortType = (formatVersion!=GlobalConfig::VCF30);
    QString separator = shortType ? ";" : ";TYPE=";
    QString typeStr = "";
    if (st!=0 && (gd.addXToNonStandardTypes || gd.replaceNLNSNames)) { // very rare case
        foreach (const QString& typeVal, aTypes) {
            bool isStandard;
            st->translate(typeVal, &isStandard);
            if (isStandard)
                typeStr += separator + typeVal.toUpper();
            else { // very-very rare case
                QString mTypeVal = typeVal;
                if (gd.replaceNLNSNames && mTypeVal.toLatin1()!=mTypeVal.toUtf8()) //non-latin
                    mTypeVal = "PREF"; // TODO m.b. use spec. value (separate for phones, emails, addresses, etc.)? define virtual defaultValue() in StandardType...
                else if (gd.addXToNonStandardTypes)
                    mTypeVal = QString("X-")+mTypeVal;
                typeStr += separator + mTypeVal;
            }
        }
    }
    else { // general case
        typeStr = separator;
        // typeStr += aTypes.join(",").toUpper(); // value list
        typeStr += aTypes.join(separator).toUpper(); // parameter list; RFC 2426 allows both form
    }
    /*
     * TODO now syncMLRef recording is switched off, because it corrupted while editing/comparing
     * need:
     * fix it;
     * make syncMLRef recording optional (some phones show it as types);
     * show it on phone/email editing triplet
    if (syncMLRef!=-1)
        typeStr += ";X-SYNCMLREF" + QString::number(syncMLRef);
    */
    return encodeValue(typeStr, 0);
}

QString VCardData::exportDate(const DateItem &item) const
{
    return
        (formatVersion!=GlobalConfig::VCF30) ?
                item.toString(DateItem::ISOBasic) : item.toString(DateItem::ISOExtended);
}

QString VCardData::exportAddress(const PostalAddress &item) const
{
    return encodeAll("ADR", &item.types, true,
                item.offBox + ";" + item.extended
        + ";" + item.street + ";" + item.city + ";" + item.region
        + ";" + item.postalCode + ";" + item.country);
}

