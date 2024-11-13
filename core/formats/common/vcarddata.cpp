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
#include <QCoreApplication>
#include <QDir>
#include <QObject>
#include <QTextCodec>
#include <QTextStream>

#include "globals.h"
#include "quotedprintable.h"
#include "vcarddata.h"

#include <iostream>

#define MAX_BASE64_LEN 74

VCardData::VCardData()
{
    useOriginalFileVersion = gd.useOriginalFileVersion;
    groupFormat = gd.groupFormat;
    formatVersion = GlobalConfig::VCF30;
    _forceVersion = false;
    typeableTags << "TEL" << "EMAIL" << "ADR" <<"PHOTO"
        << "IMPP" << "X-SIP" <<"X-CUSTOM-IM";
    unEditingTags << "LABEL" << "PRODID"
        << "X-ACCOUNT" // MyPhoneExplorer YES, embedded Android export NO
        ; // TODO other from rfc 2426 & 63...
}

void VCardData::forceVersion(GlobalConfig::VCFVersion version)
{
    _forceVersion = true;
    // Tags with possible TYPE attribute
    formatVersion = version;
}

void VCardData::unforceVersion()
{
    _forceVersion = false;
}

bool VCardData::importRecords(BStringList &lines, ContactList& list, bool append, QStringList& errors)
{
    bool recordOpened = false;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // non-standart types also may be non-latin
    QString defaultEmptyPhoneType =  Phone::standardTypes.unTranslate(gd.defaultEmptyPhoneType);
    ContactItem item;
    if (!append)
        list.clear();
    list.photoURLCount = 0;
    QString visName = "";
    QuotedPrintable::mergeLinesets(lines);
    // Logging
    QFile logFile(qApp->applicationDirPath()+QDir::separator()+"loadlog.txt");
    debugSave(logFile, "Start reading...", true);
    // Quoted-printable flag for non-standard phone/email types
    bool quotedPrintableDefault = false;
    // Collect records
    for (int line=0; line<lines.count(); line++) {
        const BString& s = lines[line];
        BString us = s.toUpper(); // Property names and parameter names are case-insensitive (RFC 6350)
        debugSave(logFile, QString("Line: ")+s, false);
        if (s.isEmpty()) // vcf can contain empty lines
            continue;
        if (us.startsWith("BEGIN:VCARD")) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
            recordOpened = true;
            item.clear();
            visName.clear();
            item.originalFormat = "VCARD";
            quotedPrintableDefault = false;
        }
        else if (us.startsWith("END:VCARD")) {
            recordOpened = false;
            item.calculateFields();
            list.push_back(item);
        }
        else {
            // Split type:value
            int scPos = s.indexOf(":");
            if (scPos==-1) {
                item.unknownTags << TagValue(s, QString(""));
                continue;
            }
            BStringList vType =splitBySC(s.left(scPos));
            BStringList vValue =splitBySC(s.mid(scPos+1));
            const QString tag = vType[0].toUpper();
            // Encoding, charset, types
            encoding = "";
            charSet = "";
            BString typeVal = ""; // for PHOTO/URI, at least
            QStringList types; // decoded types
            int syncMLRef = -1;
            for (int i=1; i<vType.count(); i++) {
                BString typeBegin = vType[i].toUpper();
                if (typeBegin.startsWith("ENCODING="))
                    encoding = typeBegin.mid(QString("ENCODING=").length());
                else if (typeBegin.startsWith("CHARSET="))
                    charSet = typeBegin.mid(QString("CHARSET=").length());
                else if (typeBegin.startsWith("TYPE=")
                      || typeBegin.startsWith("LABEL=")) {// TODO see vCard 4.0, m.b. LABEL= points to non-standard?
                    // non-standart types may be non-latin
                    typeBegin = vType[i]; // Now not uppercased
                    typeBegin.removeS("TYPE=", Qt::CaseInsensitive)
                             .removeS("LABEL=", Qt::CaseInsensitive);
                    // Here we move from encoded string to unicode
                    QString typeCand = quotedPrintableDefault
                        // Hack for non-standard types
                        ? QuotedPrintable::decode(typeBegin, codec)
                        : codec->toUnicode(typeBegin);
                    // Detect and split types, composed as value list (RFC)
                    if (typeCand.contains(",")) {
                        QStringList typesAsValueList = typeCand.split(",");
                        foreach (const QString& vlType, typesAsValueList)
                            types << vlType;
                    }
                    else // one value - it's more fast in most cases
                        types << typeCand;
                }
                else if (typeBegin.startsWith("X-CUSTOM(")) { // Non-standard non-latin types (at least, in Samsung A50 with Android 9)
                    BString typeCand = vType[i].mid(QString("X-CUSTOM(").length());
                    typeCand = typeCand.remove(typeCand.length()-1, 1);
                    QList<QByteArray> typeAttrs = typeCand.split(',');
                    foreach (const QByteArray& attr, typeAttrs) {
                        if (attr.toUpper().startsWith("ENCODING="))
                            encoding = attr.mid(QString("ENCODING=").length()).toUpper();
                        else if (attr.toUpper().startsWith("CHARSET="))
                            charSet = attr.mid(QString("CHARSET=").length());
                        else
                            typeCand = attr;
                    }
                    types <<decodeValue(typeCand, errors);
                }
                else if (typeBegin.startsWith("VALUE="))
                    // for PHOTO/URI, at least
                    typeVal = vType[i].mid(QString("VALUE=").length());
                else if (typeBegin.startsWith("X-SYNCMLREF"))
                    syncMLRef = vType[i].mid(QString("X-SYNCMLREF").length()).toInt();
                else {
                    // "TYPE=" can be omitted in some addressbooks
                    // But it also may be encoded (~~)
                    if (typeBegin.startsWith("QUOTED-PRINTABLE")
                            || typeBegin.startsWith("BASE64"))
                        encoding = typeBegin;
                    else {// type, type...
                        // Hack for non-standard types
                        if (quotedPrintableDefault)
                            types << QuotedPrintable::decode(vType[i], codec);
                        else
                            types << codec->toUnicode(vType[i]);
                    }
                }
            }
            // Remove X-prefix from non-standard types
            for (int i=0; i<types.count(); i++)
                if (types[i].startsWith("X-"))
                    types[i]=types[i].mid(2);
            // Helper for non-standard types
            if (encoding.startsWith("QUOTED-PRINTABLE", Qt::CaseInsensitive))
                    quotedPrintableDefault = true;
            if (!types.isEmpty() && !typeableTags.contains(tag))
                errors << QObject::tr("Unexpected TYPE appearance at line %1: tag %2").arg(line+1).arg(tag);
            // Possibly binary data
            QByteArray binaryData;
            if (encoding=="B" || encoding=="BASE64") {
                binaryData = vValue[0];
                while (line<lines.count()-1
                       && !(char)(lines[line+1].trimmed()).isEmpty()
                       && QChar(lines[line+1][0]).isSpace()) {
                    binaryData += lines[line+1];
                    line++;
                }
                if (lines[line+1].trimmed().isEmpty()) line++;
                binaryData = QByteArray::fromBase64(binaryData);
            } // TODO m.b. warning if binaryData not in PHOTO/SOUND...?
            // Known tags
            if (tag=="VERSION")
                item.version =decodeValue(vValue[0], errors);
            else if (tag=="REV")
                        importDate(item.lastRev,decodeValue(vValue[0], errors), errors, item.makeGenericName());
            else if (tag=="FN") {
                item.fullName =decodeValue(vValue[0], errors);
                // Name compilation for error messages
                if (visName.isEmpty() && !item.fullName.isEmpty())
                    visName = " (" + item.fullName + ")";
            }
            else if (tag=="N") {
                foreach (const BString& name, vValue)
                    item.names <<decodeValue(name, errors);
                // If empty parts not in-middle, remove it
                item.dropFinalEmptyNames();
                // Name compilation for error messages
                if (visName.isEmpty() && !item.names.isEmpty())
                    visName = " (" + item.formatNames() + ")";
            }
            else if (tag=="NOTE")
                item.description =decodeValue(vValue[0], errors);
            else if (tag=="SORT-STRING")
                item.sortString =decodeValue(vValue[0], errors);
            else if (tag=="TEL") {
                Phone phone;
                phone.value =decodeValue(vValue[0], errors);
                // Phone type(s)
                if (types.isEmpty()) {
                    if (gd.warnOnMissingTypes)
                        errors << QObject::tr("Missing phone type at line %1: %2%3").arg(line+1).arg(QString(vValue[0])).arg(visName);
                    phone.types << defaultEmptyPhoneType.toUpper();
                }
                else phone.types = types;
                if (gd.warnOnNonStandardTypes) {
                    foreach(const QString& tType, types) {
                        bool isStandard;
                        Phone::standardTypes.translate(tType, &isStandard);
                        if (!isStandard)
                            errors << QObject::tr("Non-standard phone type at line %1: %2%3").arg(line+1).arg(tType).arg(visName);
                    }
                }
                phone.syncMLRef = syncMLRef;
                item.phones << phone;
            }
            else if (tag=="EMAIL") {
                // Some phones write empty EMAIL tag even if no email (i.e SE W300i in vCard 2.1)
                if (vValue[0].isEmpty())
                    continue;
                Email email;
                email.value =decodeValue(vValue[0], errors);
                if (types.isEmpty()) // maybe, it not a bug; some devices allows email without type
                    email.types << "pref";
                else
                    email.types = types;
                email.syncMLRef = syncMLRef;
                item.emails << email;
            }
            else if (tag=="BDAY")
                importDate(item.birthday,decodeValue(vValue[0], errors), errors, item.makeGenericName());
            else if ((tag=="ANNIVERSARY") || (tag=="X-ANNIVERSARY"))
                importDate(item.anniversary,decodeValue(vValue[0], errors), errors, item.makeGenericName());
            else if (tag=="PHOTO") {
                if (typeVal.toUpper().startsWith("URI")) { // URL according vCard 3.0
                    item.photo.pType = "URL";
                    item.photo.url =decodeValue(vValue[0], errors);
                    list.photoURLCount++;
                }
                else if (encoding.startsWith("B")) { // Binary image file
                    if (encoding=="B" || encoding=="BASE64") {
                        item.photo.data = binaryData;
                    }
                    else
                        errors << QObject::tr("Unknown encoding type at line %1: %2%3").arg(line+1).arg(encoding).arg(visName);
                    if (!types.isEmpty())
                        item.photo.pType = types[0];
                    else
                        item.photo.pType = item.photo.detectFormat();
                    if (item.photo.pType.toUpper()!="JPEG" && item.photo.pType.toUpper()!="PNG")
                        errors << QObject::tr("Unsupported photo type at line %1: %2%3").arg(line+1).arg(QString(typeVal)).arg(visName);
                }
                else if (!vValue.isEmpty() && vValue[0].toLower().contains("http")) { // Google short URL
                    item.photo.pType = "URL";
                    item.photo.url = vValue[0];
                    while (line<lines.count()-1
                           && !lines[line+1].trimmed().isEmpty()
                           && QChar(lines[line+1][0]).isSpace()) {
                        item.photo.url += lines[line+1].mid(1);
                        line++;
                    }
                    list.photoURLCount++;
                }
                else
                    errors << QObject::tr("Unknown photo kind at line %1: %2").arg(line+1).arg(visName);
            }
            // Groups
            else if (tag=="CATEGORIES"
                  || tag=="X-CATEGORIES"       // some Nokia Suite versions
                  || tag=="X-GROUP-MEMBERSHIP" // Honor/Huawei
                  || tag=="X-OPPO-GROUP")      // Realme/OPPO
                // X-CATEGORIES - some Nokia Suite versions
                foreach(const BString& val, vValue) {
                    // CATEGORIES can use "," (RFC) or ";" (MPB)
                    if (val.contains(",")) {
                        QStringList grList =splitByComma(decodeValue(val, errors));
                        foreach(const QString& subVal, grList)
                            item.groups << subVal;
                    }
                    else
                        item.groups <<decodeValue(val, errors);
                }
            else if (tag=="X-NOKIA-PND-GROUP") { // Nokia NBF
                // For some groups per contact, each group wrote in separate X-NOKIA-PND-GROUP tag
                // Same for X-GROUP-MEMBERSHIP, but its encoding is standard
                QTextCodec* utf16 = QTextCodec::codecForName("UTF-16");
                QString g = utf16->toUnicode(vValue[0]);
                if (g.right(1)[0]==0) // two null bytes
                    g.remove(g.length()-1, 1);
                item.groups << g;
            }
            // Work
            else if (tag=="ORG")
                item.organization =decodeValue(vValue[0], errors);
            else if (tag=="TITLE")
                item.title =decodeValue(vValue[0], errors);
            else if (tag=="ROLE")
                item.role =decodeValue(vValue[0], errors);
            // Addresses
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
            else if ((tag=="NICKNAME") || (tag=="X-NICKNAME"))
                item.nickName =decodeValue(vValue[0], errors);
            else if (tag=="URL")
                item.url =decodeValue(vValue[0], errors);
            else if (tag=="X-JABBER") // Pre-vCard 4.0 non-standard IM tags
                item.ims << Messenger(vValue[0], "xmpp");
            else if (tag=="X-SIP") {
                item.ims << Messenger(vValue[0], "sip");
                if (!types.isEmpty())
                    item.ims.last().types << types; // POC
            }
            else if (tag=="X-ICQ")
                item.ims << Messenger(vValue[0], "icq");
            else if (tag=="X-SKYPE-USERNAME")
                item.ims << Messenger(vValue[0], "skype");
            else if ((tag=="IMPP") || (tag=="X-CUSTOM-IM")) {
                Messenger im;
                im.value =decodeValue(vValue[0], errors);
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
                item.id =decodeValue(vValue[0], errors);
                item.idType = tag;
            }
            // Unknown and un-editing tags
            else {
                QString sTag = joinBySC(vType)
                        .remove(";ENCODING=QUOTED-PRINTABLE", Qt::CaseInsensitive)
                        .remove(";CHARSET=UTF-8", Qt::CaseInsensitive);
                TagValue pair = binaryData.isEmpty() ?
                     TagValue(sTag, joinBySCAndDecode(vValue, errors))
                     :
                     TagValue(sTag, binaryData);
                // Known but un-editing tags
                if (unEditingTags.contains(tag))
                    item.otherTags << pair;
                // Unknown tags
                else
                    item.unknownTags << pair;
            }
        }
        debugSave(logFile, "Done.", false);
    }
    if (recordOpened) {
        item.calculateFields();
        list.push_back(item);
        errors << S_LAST_SECTION;
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

bool VCardData::exportRecords(BStringList &lines, const ContactList &list, QStringList& errors)
{
    foreach (const ContactItem& item, list)
        exportRecord(lines, item, errors);
    return (!list.isEmpty());
}

void VCardData::exportRecord(BStringList &lines, const ContactItem &item, QStringList& errors)
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
    BString sVersion;
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
    lines << BString("VERSION:") + sVersion;
    if (!item.lastRev.isEmpty())
        lines << BString("REV:") + exportDate(item.lastRev);
    // Known tags
    if (!item.names.isEmpty()) {
        BString seps = "";
        if (item.names.count()<MAX_NAMES && formatVersion!=GlobalConfig::VCF21)
            seps.fill(';', MAX_NAMES-item.names.count());
        lines <<encodeAll("N", 0, false,joinBySC(item.names)) + seps;
    }
    if (!item.fullName.isEmpty())
        lines <<encodeAll("FN", 0, false,sc(item.fullName));
    else if (gd.writeFullNameIsEmpty)
        lines << BString("FN:"); // default behaviour, except older phones e.g. se300
    if (!item.sortString.isEmpty())
        lines <<encodeAll("SORT-STRING", 0, false,sc(item.sortString));
    if (!item.nickName.isEmpty())
        lines <<encodeAll("NICKNAME", 0, false,sc(item.nickName));
    // vCard 3.0 at Motorola Defy saves X-NICKNAME instead it
    foreach (const Phone& ph, item.phones)
        lines << BString("TEL") + encodeTypes(ph.types, &Phone::standardTypes, ph.syncMLRef)
                  + ":" + ph.value.toLatin1();
    foreach (const Email& em, item.emails)
        lines << BString("EMAIL") +encodeTypes(em.types, &Email::standardTypes, em.syncMLRef)
                 +":"+encodeValue(em.value, 0);
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
        lines << BString("BDAY:") + exportDate(item.birthday);
    if (!item.anniversary.isEmpty()) {
        if (formatVersion>=GlobalConfig::VCF40)
            lines << BString("ANNIVERSARY:") + exportDate(item.anniversary);
        else
            lines << BString("X-ANNIVERSARY:") + exportDate(item.anniversary);
    }
    if (!item.groups.isEmpty()) {
        switch(groupFormat) {
        case GlobalConfig::gfCategories: // One tag - all groups
            lines <<encodeAll("CATEGORIES", 0, false,joinByComma(item.groups));
            break;
        case GlobalConfig::gfXGroupMembership: // Separate tag for each group
            foreach (const QString& gr, item.groups)
                lines <<encodeAll("X-GROUP-MEMBERSHIP", 0, false,cm(gr));
            break;
        case GlobalConfig::gfXOppoGroups: // Separate tag for each group
            foreach (const QString& gr, item.groups)
                lines <<encodeAll("X-OPPO-GROUP", 0, false,cm(gr));
            break;
            /*
        case GlobalConfig::gfXCategories:
            // no more info
            break;
            */
        case GlobalConfig::gfNBF: // Separate tag for each group + force UTF16
            foreach (const QString& gr, item.groups)
                lines << BString("X-NOKIA-PND-GROUP:") + BString((char*)(cm(gr).utf16()));
            break;
        case GlobalConfig::gfMPB:
            lines <<encodeAll("CATEGORIES", 0, false,joinBySC(item.groups));
            break;
        }
    }
    // Addresses
    foreach (const PostalAddress& addr, item.addrs)
        lines << exportAddress(addr);
    // Organization
    if (!item.organization.isEmpty())
        lines <<encodeAll("ORG", 0, true,sc(item.organization));
    if (!item.title.isEmpty())
        lines <<encodeAll("TITLE", 0, true,sc(item.title));
    if (!item.role.isEmpty())
        lines <<encodeAll("ROLE", 0, true,sc(item.role));
    // Internet 1
    if (!item.url.isEmpty())
        lines <<encodeAll("URL", 0, false,sc(item.url));
    // Photos
    if (item.photo.pType=="URL")
        lines << BString("PHOTO;VALUE=uri:") +encodeValue(sc(item.photo.url), 0);
    else if (!item.photo.pType.isEmpty()) {
        BString base64Line = BString("PHOTO;ENCODING=B;TYPE=")
                + item.photo.pType.toLatin1() + ":" + item.photo.data.toBase64();
        while (base64Line.length()>MAX_BASE64_LEN) {
            lines << base64Line.left(MAX_BASE64_LEN);
            base64Line.remove(0, MAX_BASE64_LEN);
            base64Line = BString(" ") + base64Line;
        }
        if (!base64Line.isEmpty())
            lines << base64Line;
        lines << "";
    }
    if (!item.description.isEmpty())
        lines <<encodeAll("NOTE", 0, true,sc(item.description));
    // Internet 2
    foreach (const Messenger& im, item.ims) {
        // Use IMPP only if vcard4 profile selected
        // RFC 4770 defines IMPP for vcard3, but some devices with vcard3 don't know it
        // TODO maybe add additional restrictions for Ancient Android 2, Soneric, iPhone, etc.
        if ((formatVersion>=GlobalConfig::VCF40))
            lines << BString("IMPP") + encodeTypes(im.types, &Messenger::standardTypes, im.syncMLRef)
                     + ":"+encodeValue(im.value, 0);
        else {
            if (im.types.contains("xmpp", Qt::CaseInsensitive))
                lines <<encodeAll("X-JABBER", 0, false, im.value);
            else if (im.types.contains("sip", Qt::CaseInsensitive)) {
                QStringList extraTypes = im.types;
                extraTypes.removeOne("sip");
                lines <<encodeAll("X-SIP", &extraTypes, false, im.value);
            }
            else if (im.types.contains("icq", Qt::CaseInsensitive))
                lines <<encodeAll("X-ICQ", 0, false, im.value);
            else if (im.types.contains("skype", Qt::CaseInsensitive))
                lines <<encodeAll("X-SKYPE-USERNAME", 0, false, im.value);
            else if (!im.types.isEmpty())
                lines << BString("X-CUSTOM-IM") +encodeTypes(im.types, &Messenger::standardTypes, im.syncMLRef)
                         + ":"+encodeValue(im.value, 0);
            else
                errors << S_ERR_UNSUPPORTED_TAG.arg(item.visibleName).arg(S_IM);
        }
    }
    // Identifier
    // TODO need support for other identifier types (apple?) and more strong detection
    if (!item.id.isEmpty() && item.id.length()>=10 && item.idType!="Sequence") // second condition separate from other ID kinds. TODO: need more strong crit.
        lines << item.idType.toLatin1() + ":" +encodeValue(item.id, QString(item.idType + ":").length());
    // Known but un-editing tags
    foreach (const TagValue& tv, item.otherTags)
            lines <<encodeAll(tv.tag.toLatin1(), 0, false, tv.value);
    // Unknown tags
    foreach (const TagValue& tv, item.unknownTags)
            lines <<encodeAll(tv.tag.toLatin1(), 0, false, tv.value);
    lines << "END:VCARD";
}

// TODO Maybe, move it into DateItem::fromString
void VCardData::importDate(DateItem &item, const QString &src, QStringList& errors, const QString& location) const
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
        if (src.startsWith("--")) { // date without year: --0415 according vCard 4.0
            item.hasYear = false;
            // TODO RFC also allows record "---12"; implement if examples will appear
            QString subSrc = src.mid(2);
            if (subSrc.length()==4 || (subSrc.length()==5 && subSrc[2]=='-'))
                item.value = QDateTime(QDate(1812, subSrc.left(2).toInt(),subSrc.right(2).toInt() ), QTime());
            else
                errors << QObject::tr("Unknown datetime format: ") + src + " (" + location + ")";
        }
        else { // date WITH year
            int sdPos = src.indexOf("-"); // - in long date format
            if (sdPos!=-1)
                item.value = QDateTime::fromString(src, "yyyy-MM-dd");
            else
                item.value = QDateTime::fromString(src, "yyyyMMdd");
        }
    }
    if (!item.value.isValid())
        errors << QObject::tr("Invalid datetime: ") + src + " (" + location + ")";
}

void VCardData::importAddress(PostalAddress &item, const QStringList& aTypes, const BStringList& values, QStringList &errors) const
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

void VCardData::debugSave(QFile &logFile, const QString& s, bool firstRec)
{
    if (gd.debugSave) {
        logFile.open(firstRec ? QIODevice::WriteOnly : QIODevice::Append);
        QTextStream ss(&logFile);
        ss << s << "\n";
        logFile.close();
    }
}

BString VCardData::exportDate(const DateItem &item) const
{
    return
        ((formatVersion!=GlobalConfig::VCF30) ?
            item.toString(DateItem::ISOBasic) :
            item.toString(DateItem::ISOExtended))
            .toLatin1();
}

BString VCardData::exportAddress(const PostalAddress &item) const
{
    return encodeAll("ADR", &item.types, true,
                sc(item.offBox) + ";" + sc(item.extended)
        + ";" + sc(item.street) + ";" + sc(item.city) + ";" + sc(item.region)
            + ";" + sc(item.postalCode) + ";" + sc(item.country));
}

BString VCardData::encodeAll(const BString &tag, const QStringList *aTypes, bool forceCharSet, const QString &value) const
{
    BString encStr = tag;
    if (aTypes)
        encStr += encodeTypes(*aTypes);
    // Encoding and charset info
    if (charSet!="UTF-8" || !encoding.isEmpty() || (forceCharSet && value.toLatin1()!=value)) {
        encStr += ";CHARSET=" + charSet.toLatin1();
        if (!encoding.isEmpty())
            encStr += ";ENCODING=" + encoding.toLatin1();
    }
    encStr += ":";
    BString valStr = encodeValue(value, encStr.length());
    // Optimize ecoding :)
    if (!valStr.contains("=") && charSet=="UTF-8" && encoding=="QUOTED-PRINTABLE") {
        encStr = tag;
        if (aTypes)
            encStr += encodeTypes(*aTypes);
        encStr += ":";
    }
    return encStr + valStr;
}

BString VCardData::encodeTypes(const QStringList &aTypes, StandardTypes* st, int /*syncMLRef*/) const
{
    bool shortType = (formatVersion!=GlobalConfig::VCF30);
    BString separator = shortType ? ";" : ";TYPE=";
    BString typeStr = "";
    if (st!=0) {
        foreach (const QString& typeVal, aTypes) {
            bool isStandard;
            st->translate(typeVal, &isStandard);
            if (isStandard)
                typeStr += separator + typeVal.toUpper().toLatin1();
            else { // very-very rare case
                BString mTypeVal;
                if (typeVal.toLatin1()!=typeVal.toUtf8() && formatVersion==GlobalConfig::VCF21) { // non-latin type name
                    switch(gd.nonLatinTypeNamesPolicy) {
                    case GlobalConfig::nltnReplaceToDefault:
                        mTypeVal = "PREF"; // TODO m.b. use spec. value (separate for phones, emails, addresses, etc.)? define virtual defaultValue() in StandardType...
                        break;
                    case GlobalConfig::nltnUseXCustom: {
                        mTypeVal = BString("X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,")
                            + encodeValue(typeVal, typeStr.length()+50) + ")";
                        break;
                    }
                    case GlobalConfig::nltnSaveAsIs:
                    default:
                        mTypeVal = typeVal.toUtf8();
                        break;
                    }
                }
                else { // latin type name
                    if (gd.addXToNonStandardTypes && !typeVal.startsWith("X-", Qt::CaseInsensitive))
                        mTypeVal = BString("X-")+typeVal.toUtf8();
                    else
                        mTypeVal = typeVal.toUtf8();
                }
                typeStr += separator + mTypeVal;
            }
        }
        return typeStr;
    }
    else { // general case
        typeStr = separator;
        // typeStr += aTypes.join(",").toUpper(); // value list
        typeStr += aTypes.join(separator).toUpper().toLatin1(); // parameter list; RFC 2426 allows both form
        return encodeValue(typeStr, 0);
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
}

