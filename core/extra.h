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
#include "bstring.h"

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

#define S_FILES_STATUS QObject::tr("Records: %1. Size: %2 byte(s)")
#define S_ATT_FILES_DIR QObject::tr("Select attached files Directory")
#define S_SAVE_ATT QObject::tr("Save attachment")

struct InnerFile {
    QString relPath, name;
    QDateTime modified;
    QByteArray content;
    InnerFile(const QString& _relPath, const QString& _name,
        const QDateTime& _modified, const QByteArray& _content);
    bool saveAs(const QString &path, QString &fatalError) const;
};

class InnerFiles: public QList<InnerFile> {
public:
    bool saveAll(const QString& dirPath, QString& fatalError) const;
    int totalSize();
};

struct Note {
    long id;
    QDateTime when;
    QString text;
    Note(long _id, const QDateTime& _when, const QString& _text);
};

class Notes: public QList<Note> {
    // some about saveall??
};

/* ExtraData, among other things, contains some message lists in various RAW formats.
 * I'm not unifying this data during file reading, because:
 * 1. Some file formats contain more than one message list, and user may want to merge it interactively in message view window.
 * 2. For some formats, decoding of full message list is a very slow operation, not needed for some users, working only with contact info.
 * 3. vMessage format contains vCard info. In C++ representation, it gives cyclic links between ContactList and DecodedMessageList.
 * 4. For MPB as read-write format, it need to convert decoded messages to PDU for save to old MyPhoneExplorer format
 * Because of this, DecodedMessageList works only by user request, before message window call
 */
struct ExtraData {
    // Common fields
    QString model;
    QDateTime timeStamp;
    BStringList vmsgSMS, pduSMS; // some file formats may contains both SMS type sets
    Notes notes; // TODO check nbf
    // MPB specific
    BStringList organizer;
    BStringList vmsgSMSArchive, pduSMSArchive;
    QList<CallInfo> calls;
    // NBU specific
    QString imei, firmware, phoneLang;
    // NBU + NBF
#ifdef WITH_MESSAGES
    QList<BinarySMS> binarySMS;
#endif
    InnerFiles files; // TODO check maybe nbu only?
    void clear();
};

#endif // EXTRA_H
