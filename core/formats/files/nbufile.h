/* Double Contact
 *
 * Module: Nokia NBU backup file import
 *
 * Copyright 2018 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 * Some part of this code ported from NbuExplorer project
 * http://sourceforge.net/projects/nbuexplorer/
 * Author: Petr Vilem, petrusek@seznam.cz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef NBUFILE_H
#define NBUFILE_H

#include <QtGlobal>
#include <QDataStream>
#include <QDateTime>
#include "fileformat.h"
#include "../common/vcarddata.h"

#define NBU_SECT_ID_SIZE 16
struct NBUSectionType
{
    enum ProcessType { None, FileSystem, Vcards, Memos, GeneralFolders, Groups, Sbackup };
    quint8 id[NBU_SECT_ID_SIZE];
    ProcessType type;
    QString name, name2;
};

class NBUFile : public FileFormat, VCardData
{
public:
    NBUFile();

    // IFormat interface
public:
    static bool detect(const QString &url);
    static QStringList supportedExtensions();
    static QStringList supportedFilters();
    bool importRecords(const QString &url, ContactList &list, bool append);
    bool exportRecords(const QString &, ContactList &);
private:
    static QDateTime getDateTime(QDataStream& stream);
    static QString getString16c(QDataStream& stream);
    static quint64 getU64(QDataStream& stream);
    static quint32 getU32(QDataStream& stream);
    static quint16 getU16(QDataStream& stream);
    static quint8 getU8(QDataStream& stream);
    NBUSectionType* findSectionType(quint8* sectID);
    bool parseFolderVcard(QDataStream& stream, ContactList &list, const QString& sectName);
    bool parseFolder(QDataStream& stream, long start, const QString& sectName, ContactList &list);
    void parseContacts (QDataStream &stream);
    inline QString trimStart(const QString& src, const QString& pattern) {
        QString res = src;
        while (res.startsWith(pattern))
            res = res.remove(0, pattern.length());
        return res;
    }
};

#endif // NBUFILE_H
