/* Double Contact
 *
 * Module: CSV file profile for full vCard data saving
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef GENERICCSVPROFILE_H
#define GENERICCSVPROFILE_H

#include <QMap>
#include "csvprofilebase.h"
#include "../common/vcarddata.h"

class TypeCounter: public QMap<QString, int> {
public:
    int totalCount();
    void append(const TypeCounter& localCounter);
};

class GenericCSVProfile: public CSVProfileBase, VCardData
{
public:
    GenericCSVProfile();
    virtual bool detect(const QStringList& header) const;
    // Read
    virtual bool parseHeader(const QStringList& header);
    virtual bool importRecord(const QStringList& row, ContactItem& item, QStringList& errors);
    // Write
    virtual QStringList makeHeader();
    virtual bool prepareExport(const ContactList &list);
    virtual bool exportRecord(QStringList& row, const ContactItem& item, QStringList& errors);
private:
    bool
      hasVersion, hasFullNames, hasNames, hasBDay, hasAnn, hasSortString,
      hasDesc, hasPhotoUrl, hasOrg, hasTitle,
      hasNick, hasUrl;
    // TODO is vCard 4.0 allow many photo?
    TypeCounter phoneTypeCombinations, emailTypeCombinations, addrTypeCombinations, imTypeCombinations;
    TypeCounter otherTags, unknownTags;
    QStringList _header;
    void clearCounters();
    // prepareExport helpers
    void checkStr(const QString& value, bool& found);
    template<class T>
    void checkTypeCombinations(const QList<T> &values, TypeCounter& combinations);
    void checkAnyTags(const QList<TagValue>& tags, TypeCounter& combinations);
    // makeHeader helpers
    void makeHeaderGroup(QStringList& header, const QString& tagStart, TypeCounter& combinations);
    inline void makeHeaderItem(QStringList& row, const QString& tag, bool condition)
        { putItem(row, tag, condition); }
    // exportRecord helpers
    void putItem(QStringList& row, const QString& value, bool condition);
    template<class T>
    void putTypeCombinations(QStringList& row, const QList<T>& data, const TypeCounter& combinations);
    void putAnyTags(QStringList& row, const QList<TagValue>& data, const TypeCounter& combinations);
};

#endif // GENERICCSVPROFILE_H
