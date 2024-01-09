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
#ifndef EXPlAYTV240PROFILE_H
#define EXPlAYTV240PROFILE_H

#include "csvprofilebase.h"


class ExplayTV240Profile: public CSVProfileBase
{
public:
    ExplayTV240Profile();
    virtual bool detect(const QStringList& header) const;
    // Read
    virtual bool importRecord(const QStringList& row, ContactItem& item, QStringList&, QString& fatalError);
    // Write
    virtual QStringList makeHeader();
    virtual bool exportRecord(QStringList& row, const ContactItem& item, QStringList& errors);
};

#endif // EXPlAYTV240PROFILE_H
