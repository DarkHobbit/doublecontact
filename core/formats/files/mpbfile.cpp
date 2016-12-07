/* Double Contact
 *
 * Module: MyPhoneExplorer backup file import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "mpbfile.h"
#include <QStringList>

const QString SECTION_BEGIN = QString("MyPhoneExplorer_ContentID:");

MPBFile::MPBFile()
    :FileFormat()
{
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
    QStringList content;
    QTextStream stream(&file);
    enum Section {
        secNotFound,
        secUnknown,
        secPhonebook,
        secCalls,
        secOrganizer,
        secNotes,
        secSMS,
        secSMSArchive
    };
    Section section = secNotFound;
    do {
        QString line = stream.readLine();
        // MPB section changes
        int secPos = line.indexOf(SECTION_BEGIN);
        // TODO check on Windows; on Linux, secPos==0
        if (secPos!=-1) {
            // qDebug() << "sP " << secPos;
            QString secName = line.mid(secPos+SECTION_BEGIN.length());
            if (secName=="Model")
                list.extra.model = stream.readLine();
            else if (secName=="TimeStamp")
                list.extra.timeStamp = stream.readLine();
            else if (secName=="Phonebook")
                section = secPhonebook;
            else if (secName=="Calls")
                section = secCalls;
            else if (secName=="Organizer")
                section = secOrganizer;
            else if (secName=="Notes")
                section = secNotes;
            else if (secName=="SMS")
                section = secSMS;
            else if (secName=="SMSArchive")
                section = secSMSArchive;
            else if (secName=="EndofData")
                break;
            else
                _errors << QObject::tr("Unsupported MPB section: ") + secName;
            if (section==secSMSArchive)
                stream.setCodec("CP1251");
            else if (section==secCalls)
                stream.setCodec("UTF-8");
        }
        // MPB section content
        else
        switch (section) {
        case secNotFound:
            _fatalError = QObject::tr("File isn't MPB file or corrupted");
            return false;
        case secUnknown:
            break;
        case secPhonebook:
            content << line;
            break;
        case secCalls: {
            QStringList cells = line.split('\t');
            if (cells.count()!=6)
                _errors << QObject::tr("Strange call item: %1, size %2")
                           .arg(line).arg(cells.count());
            if (cells.count()>=6) {
                CallInfo call;
                call.cType = cells[0];
                call.timeStamp = cells[1];
                call.duration = cells[2];
                call.number = cells[3];
                call.name = cells[4];
                list.extra.calls << call;
            }
            break;
        }
        case secOrganizer:
            list.extra.organizer << line;
            break;
        case secNotes:
            list.extra.notes << line;
            break;
        case secSMS:
            list.extra.SMS << line;
            break;
        case secSMSArchive:
            list.extra.SMSArchive << line;
        }
    } while (!stream.atEnd());
    closeFile();
    // Parse contact list
    if (content.isEmpty()) {
        // TODO maybe move this string to global for other formats
        _fatalError = QObject::tr("No contact records in this file");
        return false;
    }
    return VCardData::importRecords(content, list, true, _errors);
}

bool MPBFile::exportRecords(const QString &url, ContactList &list)
{
    //_fatalError =  S_READ_ONLY_FORMAT;
    // Check format
    if (list.extra.model.isEmpty()) {
        _fatalError = QObject::tr("MPB record allowed only for MPB source");
        return false;
    }
    QStringList content;
    // vCard data
    for (int i=0; i<list.count(); i++)
        if (list[i].version.isEmpty())
            list[i].version = "3.0"; // some MPB files not contains vCard version number   
    useOriginalFileVersion = true; // TODO force vCard 3.0 or original format? see LG Leon
    skipEncoding = true; // disable pre-encoding via VCardData::encodeValue
    forceShortType = true; // disable TYPE= before phone/email types
    if (!VCardData::exportRecords(content, list))
        return false;
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    _errors.clear();
    QTextStream stream(&file);
    // Prelude sections
    writeSectionHeader(stream, "Model");
    stream << list.extra.model;
    winEndl(stream);
    writeSectionHeader(stream, "TimeStamp");
    stream << list.extra.timeStamp;
    winEndl(stream);
    // Phone book
    writeSectionHeader(stream, "Phonebook");
    foreach (const QString& line, content) {
        stream << line;
        winEndl(stream);
    }
    winEndl(stream);
    // Call history
    writeSectionHeader(stream, "Calls");
    foreach (const CallInfo& call, list.extra.calls) {
        // Change name if was edited
        QString aboName = call.name;
        QString foundName = "";
        foreach(const ContactItem& candItem, list) {
            foreach(const Phone& candPhone, candItem.phones) {
                if (candPhone.expandNumber(gd.defaultCountry)==Phone::expandNumber(call.number, gd.defaultCountry)) {
                    foundName = candItem.fullName; // !!! see later
                    if (foundName.isEmpty())
                        foundName = candItem.formatNames(); // TODO extract makeName from calculateFields!!!
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
        stream
            << call.cType << '\t'
            << call.timeStamp << '\t'
            << call.duration << '\t'
            << call.number << '\t'
            << aboName << '\t';
        winEndl(stream);
    }
    // Interlude sections
    writeSectionHeader(stream, "Organizer");
    foreach (const QString& line, list.extra.organizer) {
        stream << line;
        winEndl(stream);
    }
    writeSectionHeader(stream, "Notes");
    foreach (const QString& line, list.extra.notes) {
        stream << line;
        winEndl(stream);
    }
    // SMS
    writeSectionHeader(stream, "SMS");
    foreach (const QString& line, list.extra.SMS) {
        stream << line;
        winEndl(stream);
    }
    // SMS archive
    writeSectionHeader(stream, "SMSArchive","CP1251"); // TODO check with various countries/locales.
    foreach (const QString& line, list.extra.SMSArchive) {
        stream << line;
        winEndl(stream);
    }
    // Coda
    writeSectionHeader(stream, "EndofData", "ISO 8859-1", false); // without endl!
    closeFile();
    return true;
}

void MPBFile::writeSectionHeader(QTextStream &stream, const QString &sectionName, const char* codecAfter, bool writeEOL)
{
    stream.setCodec("ISO 8859-1"); // to provide correct 0xff write
    stream << '\xff' << SECTION_BEGIN << sectionName;
    if (writeEOL)
        winEndl(stream);
    stream.setCodec(codecAfter);
}

void MPBFile::winEndl(QTextStream &stream)
{
    stream << (char)13 << endl;
}
