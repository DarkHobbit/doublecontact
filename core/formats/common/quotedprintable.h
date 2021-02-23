/* Double Contact
 *
 * Module: Quoted-printable encoding support
 *
 * Copyright 2019 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef QUOTEDPRINTABLE_H
#define QUOTEDPRINTABLE_H

#define MAX_QUOTED_PRINTABLE_LEN 76

#include <QString>
#include <QStringList>
#include <QTextCodec>

class QuotedPrintable
{
public:
    // Decode string
    static QString decode(const QString &src, QTextCodec* codec);
    static QString decodeFromMime(const QString &src);
    // Encode string
    static QString encode(const QString &src, QTextCodec* codec, int prefixLen);
    // Merge quoted-printable linesets
    static void mergeLinesets(QStringList& lines);
    static void mergeLines(QString& line);
private:
    // Check for soft line breaks (RFC 2045)
    static void checkSoftBreak(QString& buf, QString& lBuf, int prefixLen, int addSize, bool lastChar);
};

#endif // QUOTEDPRINTABLE_H
