/* Double Contact
 *
 * Module: CSV file profile for Sylpheed Email Client (checked on Sylpheed 3.7)
 *
 * Copyright 2024 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef SYLPHEEDPROFILE_H
#define SYLPHEEDPROFILE_H

#include <QMap>
#include "csvprofilebase.h"

class SylpheedProfile : public CSVProfileBase
{
public:
    SylpheedProfile();
    virtual bool detect(const QStringList &header) const;
    // Read
    virtual bool importRecord(const QStringList &row, ContactItem &item, QStringList &errors, QString& fatalError);
    // Write
    virtual QStringList makeHeader();
    virtual bool exportRecord(QStringList &row, const ContactItem &item, QStringList &errors);
};

#endif // SYLPHEEDPROFILE_H
