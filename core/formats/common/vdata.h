/* Double Contact
 *
 * Module: Common routines for VCard, VMessage, VCalendar...
 *
 * Copyright 2022 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef VDATA_H
#define VDATA_H

#include <QStringList>
#include "bstring.h"

struct VData
{
    VData();
    //const
    QString decodeValue(const BString& src, QStringList& errors) const;
    BString encodeValue(const QString& src, int prefixLen) const;
    QString joinBySCAndDecode(const BStringList& src, QStringList& errors);
    //static
    static BStringList splitBySC(const BString& src);
    static QString joinBySC(const QStringList& src);
    static QString joinBySC(const BStringList& src);
    static QString sc(const QString& src);
    static QStringList splitByComma(const QString& src);
    static QString joinByComma(const QStringList& src);
    static QString cm(const QString& src);
    QString encoding;
    QString charSet;
};

#endif // VDATA_H
