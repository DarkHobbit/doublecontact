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

#if QT_VERSION >= 0x050000
#include <QRegularExpression>
#endif

#include <QTextCodec>
#include "globals.h"
#include "quotedprintable.h"
#include "vdata.h"

VData::VData()
{
}
#include <iostream>
QString VData::decodeValue(const BString &src, QStringList& errors) const
{
    QTextCodec *codec; // for values
    // Charset
    if (charSet.isEmpty())
        codec = QTextCodec::codecForName("UTF-8");
    else
        codec = QTextCodec::codecForName(charSet.toLocal8Bit());
    if (!codec) {
        errors << S_UNKNOWN_CHARSET.arg(charSet);
        return "";
    }
    // Encoding
    if (encoding.isEmpty() || encoding.startsWith("8BIT", Qt::CaseInsensitive))
        return codec->toUnicode(src);
    else if (encoding.toUpper()=="QUOTED-PRINTABLE")
        return QuotedPrintable::decode(src, codec);
    else {
        std::cout << "UNK ENC " << src.data() << std::endl;
        errors << S_UNKNOWN_ENCODING.arg(encoding)+" ("+QString(src)+")";
        return "";
    }
}

BString VData::encodeValue(const QString &src, int prefixLen) const
{
    QTextCodec *codec;
    // Charset
    if (charSet.isEmpty())
        codec = QTextCodec::codecForName("UTF-8");
    else
        codec = QTextCodec::codecForName(charSet.toLocal8Bit());
    if (encoding.toUpper()=="QUOTED-PRINTABLE")
        return QuotedPrintable::encode(src, codec, prefixLen);
    else
        return codec->fromUnicode(src);
}

BStringList VData::splitBySC(const BString &src)
{
// Unfortunately, QByteArray (not QString) don't work with QRegularExpression
//#if QT_VERSION >= 0x050000
//    return src.split(QRegularExpression("(?<!\\\\);")).replaceInStrings("\\;", ";");
//#else
    QList<QByteArray> medium = src.split(';');
    BStringList res;
    foreach (const BString& s, medium) {
        if (res.isEmpty())
            res << s;
        else if (res.last().right(1)=="\\") {
            res.last().remove(res.last().length()-1, 1);
            res.last() += ";" + s;
        }
        else
            res << s;
    }
    return res;
//#endif
}

QString VData::joinBySC(const QStringList &src)
{
    return QStringList(src).replaceInStrings(";", "\\;").join(";");
}

QString VData::joinBySC(const BStringList &src)
{
    QStringList inner;
    foreach (const BString& s, src)
        inner << s;
    return joinBySC(inner);
}

QString VData::joinBySCAndDecode(const BStringList &src, QStringList& errors)
{
    QStringList inner;
    foreach (const BString& s, src)
        inner << decodeValue(s, errors);
    return joinBySC(inner);
}

QString VData::sc(const QString &src)
{
    return QString(src).replace(QString(";"), QString("\\;"));
}

QStringList VData::splitByComma(const QString &src)
{
    // TODO see RFC and m.b. proceed \\ as in splitBySC
    QStringList res = src.split(",");
    return res;
}

QString VData::joinByComma(const QStringList &src)
{
    return QStringList(src).replaceInStrings(",", "\\,").join(",");
}

QString VData::cm(const QString &src)
{
    return QString(src).replace(QString(","), QString("\\,"));
}

