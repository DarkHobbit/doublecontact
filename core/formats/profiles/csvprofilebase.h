/* Double Contact
 *
 * Module: Abstract class for CSV file profiles
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef CSVPROFILEBASE_H
#define CSVPROFILEBASE_H

#include <QMap>
#include <QString>
#include <QStringList>
#include "../../contactlist.h"
#include "../files/fileformat.h"

#define S_CSV_ROW_TOO_SHORT QObject::tr("CSV row too short for this profile at line %1")

class CSVProfileBase
{
public:
    enum QuotingPolicy { // How to write quotes
        NeverQuote,
        QuoteIfNeed, // ...if at least comma found in current cell
        AlwaysQuote
    };
    enum LineEnding {
        LFEnding,
        CRLFEnding
    };
    virtual ~CSVProfileBase() {};
    virtual bool detect(const QStringList& header) const=0;
    // Profile properties
    QString name() const;
    bool hasHeader() const;
    QString charSet() const;
    bool hasBOM() const; // only if charset is UTF*
    QuotingPolicy quotingPolicy() const;
    LineEnding lineEnding() const;
    // Read
    virtual bool parseHeader(const QStringList& header);
    virtual bool importRecord(const QStringList& row, ContactItem& item, QStringList& errors, QString& fatalError)=0;
    // Write
    virtual bool prepareExport(const ContactList &list);
    virtual QStringList makeHeader();
    virtual bool exportRecord(QStringList& row, const ContactItem& item, QStringList& errors)=0;
protected:
    // Profile properties
    QString _name, _charSet;
    bool _hasHeader, _hasBOM;
    QuotingPolicy _quotingPolicy;
    LineEnding _lineEnding;
    QMap<QString, int> columnIndexes;
    // Helpers
    bool present(const QStringList &row, int index);
    bool present(const QStringList &row, const QString& colName);
    QString value(const QStringList &row, const QString& colName);
    bool condAddPhone(const QStringList &row, ContactItem &item, int index, const QString& phType);
    bool condReadValue(const QStringList &row, int index, QString& dest);
    bool readWarning(const QStringList &row, int index, QStringList& errors);
    QString saveNamePart(const ContactItem &item, int nameIndex);
    void nextRow(QStringList& row);
public:
    QList<QStringList> auxRows; // for multi-line profiles
    // (when one contact may generate some output rows)
    // It's public. It's against OOP canon but more fast (no copy many strings each records)
};

// Macros for use exclusively in exportRecord(...) methods
#define LOSS_DATA(x, y) FileFormat::lossData(errors, item.visibleName, x, y)

#endif // CSVPROFILEBASE_H
