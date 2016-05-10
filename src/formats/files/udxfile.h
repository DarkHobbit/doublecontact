/* Double Contact
 *
 * Module: UDX (Philips Xenium) file export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef UDXFILE_H
#define UDXFILE_H


#include <QDomDocument>
#include "fileformat.h"

class UDXFile : public FileFormat, QDomDocument
{
public:
    UDXFile();
    // IFormat interface
    static bool detect(const QString &url);
    static QStringList supportedExtensions();
    static QStringList supportedFilters();
    bool importRecords(const QString &url, ContactList &list, bool append);
    bool exportRecords(const QString &url, const ContactList &list);
};

#endif // UDXFILE_H
