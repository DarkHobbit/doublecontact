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
#ifndef BSTRING_H
#define BSTRING_H

#include <QByteArray>
#include <QList>
#include <QStringList>
#include <QTextCodec>
#include <QtGlobal>

class BStringList;
class BString : public QByteArray
{
public:
    BString();
    BString(const char* str);
    BString(const char* data, int size);
    BString(const QByteArray& other);
    BStringList splitByLines() const; // Line split with CR, LF and CRLF support
    BString& removeS (const BString& str, Qt::CaseSensitivity cs = Qt::CaseSensitive);
};

class BStringList: public QList<BString>
{
public:
    QStringList toQStrings(QTextCodec* codec = 0) const;
    static BStringList fromQStrings(const QStringList& src, QTextCodec* codec = 0);
    BString joinByLines(); // TODO m.b. transfer line style
};

#endif // BSTRING_H
