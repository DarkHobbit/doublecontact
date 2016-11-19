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
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    _errors.clear();
    QStringList content;
    list.contentBefore.clear();
    list.contentAfter.clear();
    QTextStream stream(&file);
    enum Section {
        secTempBefore, // TODO m.b. in future implement other custom sections: Model, TimeStamp, Organizer, Notes, Calls, SMS
        secPhonebook,
        secTempAfter
    };
    Section section = secTempBefore;
    do {
        QString line = stream.readLine();
        switch (section) {
        case secTempBefore:
            list.contentBefore << line;
            if (line.contains("MyPhoneExplorer_ContentID:Phonebook"))
                section = secPhonebook;
            break;
        case secPhonebook:
            if (line[0]=='\xff') {
                list.contentAfter << line;
                section = secTempAfter;
            }
            else
                content << line;
            break;
        case secTempAfter:
            list.contentAfter << line;
        }
    } while (!stream.atEnd());
    closeFile();
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
