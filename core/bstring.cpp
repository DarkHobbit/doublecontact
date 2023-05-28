/* Double Contact
 *
 * Module: Byte-based encoded (m.b. not unicode) string
 *
 * Copyright 2022 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "bstring.h"

BString::BString()
{
}

BString::BString(const char *str)
    :QByteArray (str)
{
}

BString::BString(const char *data, int size)
    :QByteArray (data, size)
{
}

BString::BString(const QByteArray &other)
    :QByteArray (other)
{
}

// Line split with CR, LF and CRLF support
BStringList BString::splitByLines() const
{
    BStringList res;
    bool wasCr =  false;
    BString line;
    foreach (const char b, *this) {
        if (b=='\r') {
            res << line;
            line.clear();
            wasCr = true;
        }
        else if (b=='\n') {
            if (!wasCr) {
                res << line;
                line.clear();
            }
            wasCr = false;
        }
        else {
            line += b;
            wasCr = false;
        }
    }
    if (!line.isEmpty()) // last line
        res << line;
    return res;
}

BString &BString::removeS(const BString &str, Qt::CaseSensitivity cs)
{
    BString pattern = str;
    if (cs==Qt::CaseInsensitive)
        pattern = pattern.toUpper();
    while (true) {
        BString us = *this;
        if (cs==Qt::CaseInsensitive)
            us = us.toUpper();
        int index = us.indexOf(pattern);
        if (index==-1)
            break;
        remove(index, pattern.length());
    }
    return *this;
}

// TODO currently not used
QStringList BStringList::toQStrings(QTextCodec *codec) const
{
    QStringList res;
    foreach(const BString& s, *this) {
        if (codec)
            res << codec->toUnicode(s);
        else
            res << QString::fromUtf8(s);
    }
    return res;
}

BStringList BStringList::fromQStrings(const QStringList& src, QTextCodec *codec)
{
    BStringList res;
    foreach(const QString& s, src) {
        if (codec)
            res << codec->fromUnicode(s);
        else
            res << s.toUtf8();
    }
    return res;
}

BString BStringList::joinByLines()
{
    BString res;
    foreach (const BString& s, *this) {
        if (!res.isEmpty())
            res += '\n';
        res += s;
    }
    return res;
}
