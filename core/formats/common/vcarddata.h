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

#include <QStringList>
#include "../../contactlist.h"

class VCardData
{
public:
    VCardData();
    bool importRecords(QStringList& lines, ContactList& list, bool append, QStringList& errors);
    bool exportRecords(QStringList& lines, const ContactList& list);
    void exportRecord(QStringList& lines, const ContactItem& item);
protected:
    bool useOriginalFileVersion, skipEncoding;
private:
    QString encoding;
    QString charSet;
    GlobalConfig::VCFVersion formatVersion;
    QString decodeValue(const QString& src, QStringList& errors) const;
    void importDate(DateItem& item, const QString& src, QStringList& errors) const;
    void importAddress(PostalAddress& item, const QStringList& aTypes, const QStringList& values, QStringList& errors) const;
    QString encodeValue(const QString& src, int prefixLen) const;
    QString encodeAll(const QString& tag, const QStringList *aTypes, bool forceCharSet, const QString& value) const;
    QString encodeTypes(const QStringList& aTypes, int syncMLRef = -1) const;
    QString exportDate(const DateItem& item) const;
    QString exportAddress(const PostalAddress& item) const;
    void checkQPSoftBreak(QString& buf, QString& lBuf, int prefixLen, int addSize, bool lastChar) const;
};

#endif // VCARDDATA_H
