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
    static bool importRecords(const QStringList& lines, ContactList &list, bool append);
    static bool exportRecords(QStringList& lines, const ContactList &list);
};

#endif // VCARDDATA_H
