/* Double Contact
 *
 * Module: VCard data export/import (both for file and network media)
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef VCARDDATA_H
#define VCARDDATA_H

#include <QFile>
#include <QStringList>

#include "../../contactlist.h"
#include "bstring.h"
#include "vdata.h"

class VCardData: public VData
{
public:
    VCardData();
    void forceVersion(GlobalConfig::VCFVersion version);
    void unforceVersion();
    bool importRecords(BStringList& lines, ContactList& list, bool append, QStringList& errors);
    bool exportRecords(BStringList& lines, const ContactList& list, QStringList& errors);
    void exportRecord(BStringList& lines, const ContactItem& item, QStringList& errors);
protected:
    bool useOriginalFileVersion;
    GlobalConfig::GroupFormat groupFormat;
private:
    GlobalConfig::VCFVersion formatVersion;
    bool _forceVersion;
    QStringList typeableTags, // Tags with possible TYPE attribute
                unEditingTags; // Known but un-editing tags
    void importDate(DateItem& item, const QString& src, QStringList& errors, const QString& location) const;
    void importAddress(PostalAddress& item, const QStringList& aTypes, const BStringList& values, QStringList& errors) const;
    BString exportDate(const DateItem& item) const;
    BString exportAddress(const PostalAddress& item) const;
    BString encodeAll(const BString& tag, const QStringList *aTypes, bool forceCharSet, const QString& value) const;
    BString encodeTypes(const QStringList& aTypes, StandardTypes* st = 0, int syncMLRef = -1) const;
public:
    void debugSave(QFile& logFile, const QString& s, bool firstRec);
};

#endif // VCARDDATA_H
