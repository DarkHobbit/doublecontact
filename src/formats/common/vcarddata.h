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
    bool importRecords(QStringList& lines, ContactList& list, bool append, QStringList& errors);
    bool exportRecords(QStringList& lines, const ContactList& list);
private:
    QString encoding;
    QString charSet;
    QString decodeValue(const QString& src, QStringList& errors) const;
    void importDate(DateItem& item, const QString& src, QStringList& errors) const;
    void importAddress(PostalAddress& item, const QStringList& aTypes, const QStringList& values, QStringList& errors) const;
};

#endif // VCARDDATA_H
