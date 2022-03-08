/* Double Contact
 *
 * Module: Extra data structures (SMS, calls, inner filesystem(s), etc.)
 * for backup files (MPB, NBF, NBU)
 *
 * Copyright 2022 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QFile>
#include <QDir>
#include <QObject>

#include "globals.h"
#include "extra.h"

#ifdef WITH_CALLS
QString CallInfo::typeName() const
{
    if (cType=="DC")
        return QObject::tr("Dialed");
    else if (cType=="RC")
        return QObject::tr("Received");
    else if (cType=="MC")
        return QObject::tr("Missed");
    else
        return cType;
}
#endif

void ExtraData::clear()
{
    model.clear();
    timeStamp = QDateTime();
    organizer.clear();
    notes.clear();
    vmsgSMS.clear();
    pduSMS.clear();
#ifdef WITH_MESSAGES
    binarySMS.clear();
#endif
    vmsgSMSArchive.clear();
    pduSMSArchive.clear();
    calls.clear();
    imei.clear();
    firmware.clear();
    phoneLang.clear();
}


InnerFile::InnerFile(const QString &_relPath, const QString &_name, const QDateTime &_modified, const QByteArray &_content)
    :relPath(_relPath), name(_name), modified(_modified), content(_content)
{}

bool InnerFiles::saveAll(const QString &dirPath, QString &fatalError) const
{
    QDir d;
    if (!d.exists(dirPath)) {
        if (!d.mkpath(dirPath)) {
            fatalError = S_MKDIR_ERR.arg(dirPath);
            return false;
        }
    }
    foreach (const InnerFile& fileInfo, *this) {
        QString fileName = dirPath+QDir::separator();
        if (!fileInfo.relPath.isEmpty())
            fileName += fileInfo.relPath+QDir::separator();
        fileName += fileInfo.name;
        QFile f(fileName);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(fileInfo.content);
            f.close();
            // Set message time for attachment
#if QT_VERSION >= 0x050A00 // Qt >= 5.10
            // Setting time on-the-fly, in WriteOnly mode, not works on some platforms
            if (fileInfo.modified.isValid())
            if (f.open(QIODevice::Append)) {
                f.setFileTime(fileInfo.modified, QFileDevice::FileBirthTime);
                f.setFileTime(fileInfo.modified, QFileDevice::FileModificationTime);
                f.close();
            }
#endif
        }
        else {
            fatalError = S_WRITE_ERR.arg(fileName);
            return false;
        }
    }
    return true;
}
