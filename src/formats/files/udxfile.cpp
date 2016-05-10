/* Double Contact
 *
 * Module: UDX (Philips Xenium) file export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QTextCodec>
#include "udxfile.h"

UDXFile::UDXFile()
    :FileFormat()
{
}

bool UDXFile::detect(const QString &url)
{
    QFile file(url);
    // - file is readable
    if (!file.open( QIODevice::ReadOnly))
        return false;
    // - file is XML
    QDomDocument udx;
    if (!udx.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();
    // - root file tag must be...
    return udx.documentElement().nodeName()=="DataExchangeInfo";
}

QStringList UDXFile::supportedExtensions()
{
    return (QStringList() << "udx" << "UDX");
}

QStringList UDXFile::supportedFilters()
{
    return (QStringList() << "Philips (*.udx *.UDX)");
}

bool UDXFile::importRecords(const QString &url, ContactList &list, bool append)
{
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    // Read XML
    QString err_msg;
    int err_line, err_col;
    if (!setContent(&file, &err_msg, &err_line, &err_col)) {
        _errors << QObject::tr("Can't read content from file %1\n%2\nline %3, col %4\n")
            .arg(url).arg(err_msg).arg(err_line).arg(err_col);
        closeFile();
        return false;
    }
    closeFile();
    // Root element
    QDomElement root = documentElement();
    if (root.nodeName()!="DataExchangeInfo") {
        _errors << QObject::tr("Root node is not 'DataExchangeInfo' at file\n%1").arg(url);
        return false;
    }
    // Codepage, version, expected record count
    QDomElement recInfo = root.firstChildElement("RecordInfo");
    if (recInfo.isNull()) {
        _errors << QObject::tr("Can't find record info at file\n%1").arg(url);
        return false;
    }
    QString charSet = recInfo.firstChildElement("Encoding").text();
    if (charSet.isEmpty()) {
        _errors << QObject::tr("Warning: codepage not found, trying use UTF-8...");
        charSet = "UTF-8";
    }
    QDomElement vcfInfo = recInfo.firstChildElement("RecordOfvCard");
    QString vcVer = vcfInfo.firstChildElement("vCardVersion").text();
    int expCount = vcfInfo.firstChildElement("vCardRecord").text().toInt();
    // vCard set
    QDomElement vCard = root.firstChildElement("vCard");
    if (vCard.isNull()) {
        _errors << QObject::tr("Can't find vCard records at file\n%1").arg(url);
        return false;
    }
    QTextCodec* codec = QTextCodec::codecForName(charSet.toLocal8Bit());
    ContactItem item;
    if (!append)
        list.clear();
    QDomElement vCardInfo = vCard.firstChildElement("vCardInfo");
    while (!vCardInfo.isNull()) {
        item.clear();
        item.originalFormat = "UDX";
        item.version = vcVer; // TODO version of udx, not vcf?
        item.id = vCardInfo.firstChildElement("Sequence").text();
        QDomElement fields = vCardInfo.firstChildElement("vCardField");
        if (fields.isNull())
            _errors << QObject::tr("Can't find vCardField at sequence %1").arg(item.id);
        QDomElement field = fields.firstChildElement();
        while (!field.isNull()) {
            QString fldName = field.nodeName().toUpper();
            QString fldValue = codec->toUnicode(field.text().toLocal8Bit());
            if (fldName=="N") {
                item.names = fldValue.split(";");
                // In ALL known me udx files part before first ; was EMPTY
                if (item.names[0].isEmpty())
                    item.names.removeAt(0);
            }
            else if (fldName.startsWith("TEL")) {
                Phone phone;
                phone.number = fldValue;
                if (fldName=="TEL")
                    phone.tTypes << "CELL";
                else if (fldName=="TELHOME")
                    phone.tTypes << "HOME";
                else if (fldName=="TELWORK")
                    phone.tTypes << "WORK";
                else if (fldName=="TELFAX")
                    phone.tTypes << "FAX";
                else
                    _errors << QObject::tr("Unknown phone type: %1 (%2)").arg(phone.number).arg(item.names[0]);
                item.phones.push_back(phone);
            }
            else if (fldName=="ORGNAME")
                item.organization = fldValue;
            else if (fldName=="BDAY")
                item.birthday.value = QDateTime::fromString("yyyyMMdd");
            else if (fldName=="EMAIL") {
                Email email;
                email.address = fldValue;
                email.emTypes << "pref";
                item.emails.push_back(email);
            }
            else
                _errors << QObject::tr("Unknown vCard field: %1").arg(fldName);
            field = field.nextSiblingElement();
        }
        item.calculateFields();
        list.push_back(item);
        vCardInfo = vCardInfo.nextSiblingElement();
    }
    if (list.count()!=expCount)
        _errors << QObject::tr("%1 records read, %2 expected").arg(list.count()).arg(expCount);
    // Unknown tags statistics
    int totalUnknownTags = 0;
    foreach (const ContactItem& _item, list)
        totalUnknownTags += _item.unknownTags.count();
    if (totalUnknownTags)
        _errors << QObject::tr("%1 unknown tags found").arg(totalUnknownTags);
    // Ready
    return (!list.isEmpty());
}

bool UDXFile::exportRecords(const QString &url, const ContactList &list)
{
    // TODO
}
