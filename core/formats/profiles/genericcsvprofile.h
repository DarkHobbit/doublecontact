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

#include "csvprofilebase.h"


class GenericCSVProfile: public CSVProfileBase
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
    bool hasFullNames, hasNames, hasBDay, hasAnn, hasSortString;
    int phoneCount, emailCount, addrCount;
    QStringList _header;
    void clearCounters();
};

#endif // GENERICCSVPROFILE_H
