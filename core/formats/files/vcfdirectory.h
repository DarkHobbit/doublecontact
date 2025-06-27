/* Double Contact
 *
 * Module: VCard directory export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef VCFDIR_H
#define VCFDIR_H

#include "fileformat.h"

class VCFDirectory : public FileFormat
{
public:
    VCFDirectory();
    virtual ~VCFDirectory();

    // IFormat interface
public:
    virtual bool setCharSet(const QString& value);
    bool importRecords(const QString &url, ContactList &list, bool append);
    bool exportRecords(const QString &url, ContactList &list);
private:
    QString charSet;
};

#endif // VCFDIR_H
