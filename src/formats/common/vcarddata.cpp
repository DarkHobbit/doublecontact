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
#include <QDebug>

bool VCardData::importRecords(QStringList &lines, ContactList& list, bool append, QStringList& errors)
{
    bool recordOpened = false;
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
            // Encoding and charset
            QString encoding;
            QString charSet;
            for (int i=1; i<vType.count(); i++) {
                if (vType[i].startsWith("ENCODING="))
                    encoding = vType[i].mid(9);
                else if (vType[i].startsWith("CHARSET="))
                    charSet = vType[i].mid(8);
                else
                    item.unknownTags.push_back(TagValue(vType.join(";"), vValue.join(";"))); // TODO partiallyKnownTag?
            }
            // Known tags
            const QString tag = vType[0].toUpper();
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
                if (vType.count()<2)
                    errors << QObject::tr("Missing phone type at line %1").arg(line+1);
                QString typeAttrs;
                for (int i=1; i<vType.count(); i++) {
                    if (vType[i].startsWith("TYPE=", Qt::CaseInsensitive))
                        typeAttrs += vType[i].mid(5) + ((i<vType.count()-1) ? ";" : "");
                    else
                        item.unknownTags.push_back(TagValue(vType.join(";"), vValue.join(";"))); // TODO partiallyKnownTag?
                }
                if (!phone.typeFromString(typeAttrs))
                    errors << QObject::tr("Unknown or missing phone type at line %1").arg(line+1);
                item.phones.push_back(phone);
            }

            // TODO

            // Unknown tags
            else
                item.unknownTags.push_back(TagValue(vType.join(";"), vValue.join(";")));
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
    // Charset
    QTextCodec *codec;
    if (charSet.isEmpty()) // TODO not tested on Linux
        codec = QTextCodec::codecForName("UTF-8");
    else
        codec = QTextCodec::codecForName(charSet.toLocal8Bit());
    if (!codec) {
        errors << QObject::tr("Unknown encoding");
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
        errors << QObject::tr("Unknown encoding");
        return "";
    }
}
