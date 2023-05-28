/* Double Contact
 *
 * Module: MyPhoneExplorer backup file import
 *
 * Copyright 2016-2022 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include <qglobal.h>
#include <QStringList>
#include <QTextStream>
#include "bstring.h"
#include "corehelpers.h"
#include "mpbfile.h"

#define S_ERR_NOT_MPB QObject::tr("File isn't MPB file or corrupted")
const BString SECTION_BEGIN = BString("MyPhoneExplorer_ContentID:");

MPBFile::MPBFile()
    :FileFormat()
{
}

bool MPBFile::detect(const QString &url)
{
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QTextStream stream(&f);
    QString line = stream.readLine();
    f.close();
    return line.contains(SECTION_BEGIN);
}

QStringList MPBFile::supportedExtensions()
{
    return (QStringList() << "mpb" << "MPB");
}

QStringList MPBFile::supportedFilters()
{
    return (QStringList() << "MyPhoneExplorer Backup (*.mpb *.MPB)");
}

bool MPBFile::importRecords(const QString &url, ContactList &list, bool append)
{
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    _errors.clear();
    if (!append) // not in VCardData::importRecords; else extra data will be lost
        list.clear();
    // Read file
    QByteArray allS = file.readAll();
    closeFile();
    // Pre-check for very large alien file
    if (!allS.left(SECTION_BEGIN.length()+2).contains(SECTION_BEGIN)) {
        _fatalError = S_ERR_NOT_MPB;
        return false;
    }
    // Split and merge sections
    QList<QByteArray> all = allS.split(0xFF);
    if (all.isEmpty()) {
        _fatalError = S_ERR_NOT_MPB;
        return false;
    }
    for (int i=all.count()-1; i>=0; i--) {
        if (all[i].isEmpty())
            all.removeAt(i);
        else if (i>0 && !all[i].startsWith(SECTION_BEGIN)) {
            all[i-1] += 0xFF;
            all[i-1] += all[i];
            all.removeAt(i);
        }
    }
    if (!all.first().startsWith(SECTION_BEGIN)) {
        _fatalError = S_ERR_NOT_MPB;
        return false;
    }
    // MPB sections
    bool res = false;
    // TODO check on not-mpb file
    foreach (const BString& sec, all) {
        BStringList section = sec.splitByLines();
        QString secName = QString(section.first().mid(SECTION_BEGIN.length()));
        section.removeFirst();
        if (secName=="EndofData")
            break;
        if (section.isEmpty()) {
            _errors << QObject::tr("MPB section without content: %1").arg(secName);
            continue;
        }
        if (secName=="Model")
            list.extra.model = QString::fromUtf8(section.last());
        else if (secName=="TimeStamp")
            list.extra.timeStamp = DateItem::readISOExtDateTimeWithZone(section.last());
        else if (secName=="Phonebook")
            res = VCardData::importRecords(section, list, true, _errors);
        else if (secName=="Calls") {
            foreach(const BString& line, section) {
                QStringList cells = QString::fromUtf8(line).split('\t');
                if (cells.count()!=6)
                    _errors << QObject::tr("Strange call item: %1, size %2")
                               .arg(QString::fromUtf8(line)).arg(cells.count());
                if (cells.count()>=6) {
                    CallInfo call;
                    call.cType = cells[0];
                    call.timeStamp = cells[1];
                    call.duration = cells[2];
                    call.number = cells[3];
                    call.name = cells[4];
                    list.extra.calls << call;
                }
            }
        }
        else if (secName=="Organizer")
            list.extra.organizer = section;
        // TODO parser
        else if (secName=="Notes")
            foreach(const BString& line, section)
                list.extra.notes << Note(0, QDateTime::currentDateTime(), QString::fromUtf8(line));
                // TODO нормальный VNOTE-парсер
        // PDU SMS
        else if (secName=="SMS") {
            list.extra.pduSMS = section;
        }
        else if (secName=="SMSArchive") {
            list.extra.pduSMSArchive = section;
        }
        // VMessage SMS
        else if (secName=="Messages") {
            list.extra.vmsgSMS = section;
        }
        else if (secName=="MessageArchive") {
            list.extra.vmsgSMSArchive = section;
        }
        else
            _errors << QObject::tr("Unsupported MPB section: %1").arg(secName);
    }
    // Check contact list
    if (!res) {
        // TODO maybe move this string to global for other formats
        _fatalError = QObject::tr("No contact records in this file");
        return false;
    }
    return true;
}

bool MPBFile::exportRecords(const QString &url, ContactList &list)
{
    //_fatalError =  S_READ_ONLY_FORMAT;
    // Check format
    if (list.extra.model.isEmpty()) {
        _fatalError = QObject::tr("MPB record allowed only for MPB source");
        return false;
    }
    // Warning on Sony Ericsson
    if (list.extra.model.contains("Sony")||list.extra.model.contains("Eric")) // TODO remove, when test
        _errors << "Program was tested only on Android MPB files, not SonyEricsson. Please, contact author";
    BStringList content;
    // vCard data
    for (int i=0; i<list.count(); i++)
        if (list[i].version.isEmpty()) // some MPB files not contains vCard version number.
            list[i].version = "4.0";
    useOriginalFileVersion = false;
    /* TODO not work now
    skipEncoding = true; // disable pre-encoding via VCardData::encodeValue
    */
    groupFormat = GlobalConfig::gfMPB;
    forceVersion(GlobalConfig::VCF40); // MPB vCard section is vCard 4.0 without VERSION tag
    if (!VCardData::exportRecords(content, list, _errors))
        return false;
    unforceVersion();
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    _errors.clear();
    // Prelude sections
    writeSectionHeader(file, "Model");

    file.write(list.extra.model.toUtf8().data());
    winEndl(file);
    writeSectionHeader(file, "TimeStamp");
    
file.write(DateItem::writeISOExtDateTimeWithZone(list.extra.timeStamp).toLatin1().data());
    winEndl(file);
    // Phone book
    writeSectionHeader(file, "Phonebook");
    foreach (const BString& line, content) {
        file.write(line);
        winEndl(file);
    }
    winEndl(file);
    // Call history
    writeSectionHeader(file, "Calls");
    foreach (const CallInfo& call, list.extra.calls) {
        // Change name if was edited
        QString aboName = call.name;
        QString foundName = "";
        foreach(const ContactItem& candItem, list) {
            foreach(const Phone& candPhone, candItem.phones) {
                if (candPhone.expandNumber(gd.defaultCountryRule)==Phone::expandNumber(call.number, gd.defaultCountryRule)) {
                    foundName = candItem.makeGenericName();
                    break;
                }
            }
            if (!foundName.isEmpty()) break;
        }
        if (!foundName.isEmpty()) {
            if (aboName != foundName && !foundName.isEmpty())
                _errors << QObject::tr("Name for number %1 changed from %2 to %3")
                           .arg(call.number).arg(aboName).arg(foundName);
            aboName = foundName;
        }
        else if (!aboName.isEmpty()) {
            foreach(const ContactItem& candItem, list) {
                if (candItem.fullName==aboName) {
                    foundName = candItem.fullName;
                    break;
                }
            }
            if (foundName.isEmpty())
                _errors << QObject::tr("Number %1 without original name not found in addressbook").arg(call.number);
            else
                _errors << QObject::tr("Number %1 not found in addressbook. Original name (%2) saved").arg(call.number).arg(aboName);
        }
        // Write call item
        QString callLine = QString("%1\t%2\t%3\t%4\t%5\t")
            .arg(call.cType).arg(call.timeStamp).arg(call.duration).arg(call.number).arg(aboName);
        file.write(callLine.toUtf8().data());
        winEndl(file);
    }
    // Interlude sections
    writeSectionHeader(file, "Organizer");
    foreach (const BString& line, list.extra.organizer) {
        file.write(line);
        winEndl(file);
    }
    writeSectionHeader(file, "Notes");
    // TODO нормальный парсер
    foreach (const Note& n, list.extra.notes) {
        file.write(n.text.toUtf8().data());
        winEndl(file);
    }
    // PDU SMS
    writeSectionHeader(file, "SMS");
    foreach (const BString& line, list.extra.pduSMS) {
        file.write(line);
        winEndl(file);
    }
    // PDU SMS archive
    writeSectionHeader(file, "SMSArchive");
    foreach (const BString& line, list.extra.pduSMSArchive) {
        file.write(line);
        winEndl(file);
    }
    // VMessage SMS
    if (!list.extra.vmsgSMS.isEmpty()) { // Old MyPhoneExplorer version may be not compatible with this section
        writeSectionHeader(file, "Messages");
        foreach (const BString& line, list.extra.vmsgSMS) {
            file.write(line);
            winEndl(file);
        }
    }
    // VMessage SMS archive
    if (!list.extra.vmsgSMSArchive.isEmpty()) { // Old MyPhoneExplorer version may be not compatible with this section
        writeSectionHeader(file, "MessageArchive");
        foreach (const BString& line, list.extra.vmsgSMSArchive) {
            file.write(line);
            winEndl(file);
        }
    }
    // Coda
    writeSectionHeader(file, "EndofData", false); // without endl!
    closeFile();
    return true;
}

void MPBFile::writeSectionHeader(QIODevice &out, const BString &sectionName, bool writeEOL)
{
    out.write("\xff");
    out.write(SECTION_BEGIN);
    out.write(sectionName);
    if (writeEOL)
        winEndl(out);
}

void MPBFile::winEndl(QIODevice &out)
{
    out.write("\r\n");
}
