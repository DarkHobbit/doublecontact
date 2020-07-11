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

class VCardData
{
public:
    VCardData();
    void forceVersion(GlobalConfig::VCFVersion version);
    void unforceVersion();
    void setSkipCoding(bool _skipEncoding, bool _skipDecoding);
    bool importRecords(QStringList& lines, ContactList& list, bool append, QStringList& errors);
    bool exportRecords(QStringList& lines, const ContactList& list, QStringList& errors);
    void exportRecord(QStringList& lines, const ContactItem& item, QStringList& errors);
protected:
    bool useOriginalFileVersion, skipEncoding, skipDecoding;
    GlobalConfig::GroupFormat groupFormat;
private:
    QString encoding;
    QString charSet;
    GlobalConfig::VCFVersion formatVersion;
    bool _forceVersion;
    QString decodeValue(const QString& src, QStringList& errors) const;
    void importDate(DateItem& item, const QString& src, QStringList& errors, const QString& location) const;
    void importAddress(PostalAddress& item, const QStringList& aTypes, const QStringList& values, QStringList& errors) const;
    QString encodeValue(const QString& src, int prefixLen) const;
    QString encodeAll(const QString& tag, const QStringList *aTypes, bool forceCharSet, const QString& value) const;
    QString encodeTypes(const QStringList& aTypes, StandardTypes* st = 0, int syncMLRef = -1) const;
    QString exportDate(const DateItem& item) const;
    QString exportAddress(const PostalAddress& item) const;
    QStringList splitBySC(const QString& src);
    QString joinBySC(const QStringList& src) const;
    QString sc(const QString& src) const;
    QStringList splitByComma(const QString& src);
    QString joinByComma(const QStringList& src) const;
    QString cm(const QString& src) const;
    void debugSave(QFile& logFile, const QString& s, bool firstRec);
};

#endif // VCARDDATA_H
