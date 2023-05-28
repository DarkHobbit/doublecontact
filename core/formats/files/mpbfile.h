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

#ifndef MPBFILE_H
#define MPBFILE_H

#include "fileformat.h"
#include "../common/vcarddata.h"

class MPBFile : public FileFormat, VCardData
{
public:
    MPBFile();

    // IFormat interface
public:
    static bool detect(const QString &url);
    static QStringList supportedExtensions();
    static QStringList supportedFilters();
    bool importRecords(const QString &url, ContactList &list, bool append);
    bool exportRecords(const QString &url, ContactList &list);
private:
    void writeSectionHeader(QIODevice& out, const BString& sectionName,
        bool writeEOL=true);
    void winEndl(QIODevice & out);
};

#endif // MPBFILE_H
