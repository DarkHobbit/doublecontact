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
#include <QTextStream>
#include <QSet>
#include "udxfile.h"

UDXFile::UDXFile()
    :FileFormat(), QDomDocument("DataExchangeInfo")
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
        _errors << QObject::tr("Can't find 'RecordInfo' tag at file\n%1").arg(url);
        return false;
    }
    QString charSet = recInfo.firstChildElement("Encoding").text();
    if (charSet.isEmpty()) {
        _errors << QObject::tr("Warning: codepage not found, trying use UTF-8...");
        charSet = "UTF-8";
    }
    QString udxVer = recInfo.firstChildElement("UdxVersion").text();
    if (udxVer.isEmpty()) {
        _errors << QObject::tr("Warning: udx version not found, treat as 1.0...");
        udxVer = "1.0";
    }
    QDomElement vcfInfo = recInfo.firstChildElement("RecordOfvCard");
    QString vcVer = vcfInfo.firstChildElement("vCardVersion").text();
    int expCount = vcfInfo.firstChildElement("vCardRecord").text().toInt();
    // vCard set
    QDomElement vCard = root.firstChildElement("vCard");
    if (vCard.isNull()) {
        _errors << QObject::tr("Can't find 'vCard' records at file\n%1").arg(url);
        return false;
    }
    // QTextCodec* codec = QTextCodec::codecForName(charSet.toLocal8Bit()); TODO not works on windows
    ContactItem item;
    if (!append)
        list.clear();
    QDomElement vCardInfo = vCard.firstChildElement("vCardInfo");
    while (!vCardInfo.isNull()) {
        item.clear();
        item.originalFormat = "UDX";
        item.version = udxVer;
        item.subVersion = vcVer;
        item.id = vCardInfo.firstChildElement("Sequence").text();
        QDomElement fields = vCardInfo.firstChildElement("vCardField");
        if (fields.isNull())
            _errors << QObject::tr("Can't find 'vCardField' at sequence %1").arg(item.id);
        QDomElement field = fields.firstChildElement();
        while (!field.isNull()) {
            QString fldName = field.nodeName().toUpper();
            QString fldValue = field.text(); // codec->toUnicode(field.text().toLocal8Bit()); TODO not works on windows
            if (fldName=="N") {
                item.names = fldValue.split(";");
                // In ALL known me udx files part before first ; was EMPTY
                if (item.names[0].isEmpty())
                    item.names.removeAt(0);
                // If empty parts not in-middle, remove it
                item.dropFinalEmptyNames();
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
                item.birthday.value = QDateTime::fromString(fldValue, "yyyyMMdd");
            else if (fldName=="EMAIL") {
                Email email;
                email.address = fldValue;
                email.emTypes << "pref";
                item.emails.push_back(email);
            }
            else
                _errors << QObject::tr("Unknown 'vCardfield' type: %1").arg(fldName);
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

bool UDXFile::exportRecords(const QString &url, ContactList &list)
{
    QDomDocument::clear();
    QDomElement root = createElement("DataExchangeInfo");
    appendChild(root);
    // Original format also was UDX?
    bool wasUDX = false;
    if (!list.isEmpty())
        if (list[0].originalFormat=="UDX")
            wasUDX = true;
    // XML header
    QDomNode node = createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    insertBefore(node, firstChild());
    // UDX header
    QDomElement recInfo = addElement(root, "RecordInfo");
    addElement(recInfo, "VendorInfo", "VendorUDX");
    addElement(recInfo, "DeviceInfo", "DeviceUDX");
    if (wasUDX)
        addElement(recInfo, "UdxVersion", list[0].version);
    else
        addElement(recInfo, "UdxVersion", "1.0");
    addElement(recInfo, "UserAgent", "AgentUDX");
    addElement(recInfo, "UserInfo", "UserUDX");
    addElement(recInfo, "Encoding", "UTF-8");
    addElement(recInfo, "FileSize", "          "); // strongly 10 spaces! (~~)
    addElement(recInfo, "Date", QDate::currentDate().toString("dd.MM.yyyy"));
    addElement(recInfo, "Language","CHS"); // wtf, but this code was in real russian-language udx
    QDomElement vcfInfo = addElement(recInfo, "RecordOfvCard");
    if (wasUDX)
        addElement(vcfInfo, "vCardVersion", list[0].subVersion);
    else
        addElement(vcfInfo, "vCardVersion", "2.1");
    addElement(vcfInfo, "vCardRecord", QString::number(list.count()));
    addElement(vcfInfo, "vCardLength", "          "); // strongly 10 spaces! (~~)
    addElement(recInfo, "RecordOfvCalendar");
    addElement(recInfo, "RecordOfSMS");
    addElement(recInfo, "RecordOfMMS");
    addElement(recInfo, "RecordOfEmail");
    // Parent tag for all records
    QDomElement vCard = addElement(root, "vCard");
    // Add missing sequences to records
    int maxSeq = 0;
    if (wasUDX) { // wasUDX - simply add missing
        foreach (const ContactItem& item, list)
            if (item.id.toInt()>maxSeq)
                    maxSeq = item.id.toInt();
        QSet<int> seqs;
        for (int i=0; i<list.count(); i++) {
            ContactItem& item = list[i];
            int currentID = item.id.toInt();
            if (currentID<1) // simply missing
                item.id = QString::number(++maxSeq);
            if (seqs.contains(currentID)) { // duplicate?
                _errors << QObject::tr("Warning: contact %1, duplicate id %2 changed to %3")
                     .arg(item.visibleName).arg(currentID).arg(++maxSeq);
                item.id = QString::number(maxSeq);
            }
            seqs.insert(item.id.toInt());
        }
    }
    else { // if original wasn't UDX, completely renumerate all
        for (int i=0; i<list.count(); i++)
            list[i].id = QString::number(i+1);
        maxSeq = list.count();
    }
    // Write all records, sorted by id
    for(int i=1; i<=maxSeq; i++) {
        int index = list.findById(QString::number(i));
        if (index==-1)
            continue;
        ContactItem& item = list[index];
        QDomElement vCardInfo = addElement(vCard, "vCardInfo");
        addElement(vCardInfo, "Sequence", item.id);
        QDomElement vCardField = addElement(vCardInfo, "vCardField");
        // Names
        // TODO check if first file is surname on real phone (also has description in last test file)
        addElement(vCardField, "N", QString(";")+item.names.join(";"));
        // Phones
        foreach (const Phone& ph, item.phones) {
            if (ph.tTypes.contains("CELL", Qt::CaseInsensitive))
                addElement(vCardField, "TEL", ph.number);
            else if (ph.tTypes.contains("HOME", Qt::CaseInsensitive))
                addElement(vCardField, "TELHOME", ph.number);
            else if (ph.tTypes.contains("WORK", Qt::CaseInsensitive))
                addElement(vCardField, "TELWORK", ph.number);
            else if (ph.tTypes.contains("FAX", Qt::CaseInsensitive))
                addElement(vCardField, "TELFAX", ph.number);
            else if (ph.tTypes.join(";").toUpper()!="PREF") {
                addElement(vCardField, "TEL", ph.number);
                _errors << QObject::tr("Warning: contact %1, unknown tel type:\n%2\n saved as cellular")
                     .arg(item.visibleName).arg(ph.tTypes.join(";"));
            }
        }
        // Emails
        foreach (const Email& em, item.emails)
            addElement(vCardField, "EMAIL", em.address);
        // TODO what if save some EMAIL tags? (also some one-type TEL)
        // Organization/title
        if (!item.organization.isEmpty()) {
            QString org = item.organization;
            if (!item.title.isEmpty())
                org += ", " + item.title;
            addElement(vCardField, "ORGNAME", org);
        }
        // Birthday
        if (item.birthday.value.isValid())
            addElement(vCardField, "BDAY", item.birthday.value.toString("yyyyMMdd"));
        // TODO maybe some phones support time in udx? need search specs, and maybe need use DateItem::toString() here
        // but check format, - and T, maybe it's vCard 2.1
        if (item.birthday.hasTime)
            _errors << QObject::tr("Warning: contact %1 has time (%2) in birthday, not implemented in UDX reader")
                 .arg(item.visibleName).arg(item.birthday.value.toString("hh:mm:ss"));
        // TODO warn on address
        if ((!item.photo.isEmpty()) || (!item.photoUrl.isEmpty()))
            _errors << QObject::tr("Warning: contact %1 has photo, not implemented in UDX").arg(item.visibleName);
        if (!item.description.isEmpty())
            _errors << QObject::tr("Warning: contact %1 has description, not implemented in UDX").arg(item.visibleName);
        if (!item.title.isEmpty())
            _errors << QObject::tr("Warning: contact %1 has job title, not implemented in UDX").arg(item.visibleName);
        if (!item.anniversaries.isEmpty())
            _errors << QObject::tr("Warning: contact %1 has anniversaries, not implemented in UDX").arg(item.visibleName);
        // Here place warning on all other udx-unsupported things
    }
    QString content = toString(0);
    // Add left-aligned file size, completed to 10 characters
    int fsPos = content.indexOf("<FileSize>")+QString("<FileSize>").length();
    int fileSize = content.toLocal8Bit().size();
    content.replace(fsPos, 10, QString("%1").arg(fileSize, -10, 10, QChar(' ')));
    // Add left-aligned vcard length, completed to 10 characters
    int vclPos = content.indexOf("<vCardLength>")+QString("<vCardLength>").length();
    int vcLength = content.indexOf("</vCard>")-content.indexOf("<vCard>")+QString("</vCard>").length();
    content.replace(vclPos, 10, QString("%1").arg(vcLength, -10, 10, QChar(' ')));
    // Write to file
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    QTextStream stream(&file);
    stream << content;
    closeFile();
    return true;
}

QDomElement UDXFile::addElement(QDomElement &parent, const QString &tagName, const QString &tagValue)
{
    QDomElement e = createElement(tagName);
    if (!tagValue.isEmpty()) {
        QDomText t = createTextNode(tagValue);
        e.appendChild(t);
    }
    parent.appendChild(e);
    return e;
}
