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

#ifndef EXTRA_H
#define EXTRA_H

#include <QDateTime>
#include <QStringList>

struct CallInfo {
    QString cType, timeStamp, duration, number, name;
#ifdef WITH_CALLS
    QString typeName() const;
#endif
};

#ifdef WITH_MESSAGES
struct BinarySMS { //primarilly for Nokia Prefdef messages
    QString name;
    QByteArray content;
};
#endif

struct InnerFile {
    QString relPath, name;
    QDateTime modified;
    QByteArray content;
    InnerFile(const QString& _relPath, const QString& _name,
        const QDateTime& _modified, const QByteArray& _content);
};

class InnerFiles: public QList<InnerFile> {
public:
    bool saveAll(const QString& dirPath, QString& fatalError) const;
};

struct ExtraData {
    // Common fields
    QString model;
    QDateTime timeStamp;
    QStringList vmsgSMS, pduSMS; // some file formats may contains both SMS type sets
    // MPB specific
    QStringList organizer, notes;
    QStringList vmsgSMSArchive, pduSMSArchive;
    QList<CallInfo> calls;
    // NBU specific
    QString imei, firmware, phoneLang;
    // NBU + NBF
#ifdef WITH_MESSAGES
    QList<BinarySMS> binarySMS;
#endif
    void clear();
};

#endif // EXTRA_H
