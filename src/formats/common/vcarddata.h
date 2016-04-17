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
    static bool importRecords(QStringList& lines, ContactList& list, bool append, QStringList& errors);
    static bool exportRecords(QStringList& lines, const ContactList& list);
private:
    static QString decodeValue(const QString& src, const QString& encoding, const QString& charSet, QStringList& errors);
    static void importDate(DateItem& item, const QString& src, QStringList& errors);
    // TODO before export: maybe make class not static and move here encoding, charSet and errors as members
};

#endif // VCARDDATA_H
