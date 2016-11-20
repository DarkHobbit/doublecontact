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
#include <QTextStream>

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
    const QString SECTION_BEGIN = QString("MyPhoneExplorer_ContentID:");
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    _errors.clear();
    if (!append)
        list.extra.clear();
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
        if (line.contains(SECTION_BEGIN)) {
            QString secName = line.mid(SECTION_BEGIN.length());
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
                call.wtf = cells[2];
                call.number = cells[3];
                call.name = cells[4];
                list.extra.calls << call;
            }
            break;
        }
        case secOrganizer:
            list.extra.organizer << line;
        case secNotes:
            list.extra.notes << line;
        case secSMS:
            list.extra.SMS << line;
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
    return VCardData::importRecords(content, list, append, _errors);
}

bool MPBFile::exportRecords(const QString&, ContactList&)
{
    _fatalError =  S_READ_ONLY_FORMAT;
    return false;
}
