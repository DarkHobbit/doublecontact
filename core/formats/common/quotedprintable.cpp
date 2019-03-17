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

#include <QByteArray>
#include "quotedprintable.h"

QString QuotedPrintable::decode(const QString &src, QTextCodec *codec)
{
    QByteArray res;
    bool ok;
    for (int i=0; i<src.length(); i++) {
        if (src[i]=='=') {
            if (i<src.length()-1)
                if (src[i+1]==' ') i++; // sometime bad space after = appears
            const quint8 code = src.mid(i+1, 2).toInt(&ok, 16);
            res.append(code);
            i += 2;
        }
        else
            res += src[i];
    }
    return codec->toUnicode(res);
}

QString QuotedPrintable::encode(const QString &src, QTextCodec *codec, int prefixLen)
{
    // We can't apply codec->fromUnicode to entire string, because
    // we must find ascii-able characters, but variable character length
    // may cause false match. We must check EACH character.
    QString buf, lBuf;
    for (int i=0; i<src.count(); i++) {
        QChar ch = src[i];
        QByteArray bytes = codec->fromUnicode(ch);
        // Can we use Literal representation?
        // Rule 2 (RFC 2045). Literal representation
        bool useLiteral = (ch>=33 && ch<=126 && ch!=61);
        // Rule 3. Spaces
        if (ch==0x20 || ch==0x09)
            useLiteral = i<src.count()-1;
        // Represent!
        bool lastChar = i==src.count()-1;
        if (useLiteral) {
            QuotedPrintable::checkSoftBreak(buf, lBuf, prefixLen, 1, lastChar);
            lBuf += ch;
        }
        else { // Rule 1. General 8bit representation
            for(int j=0; j<bytes.count(); j++) {
                QuotedPrintable::checkSoftBreak(buf, lBuf, prefixLen, 3, lastChar && j==bytes.count()-1);
                QString hex = QString::number((uchar)bytes[j], 16).toUpper();
                if (hex.length()==1)
                    hex = QString("0")+hex;
                lBuf += "=" + hex;
            }
        }
    }
    if (!lBuf.isEmpty()) {
        if (!buf.isEmpty())
            buf += "=\x0d\n";
        buf += lBuf;
    }
    // Rule 4. Line Breaks - apply in caller
    return buf;
}

void QuotedPrintable::mergeLinesets(QStringList &lines)
{
    for (int i=0; i<lines.count(); i++) {
        if (i>=lines.count()) break; // need, because count changed inside this cycle
        if (lines[i].contains("QUOTED-PRINTABLE", Qt::CaseInsensitive))
            while (lines[i].right(1)=="=" && i<lines.count()-1) {
                lines[i].remove(lines[i].length()-1, 1);
                if (lines[i+1].left(1)=="\t") // Folding by tab, for example in Mozilla Thunderbird VCFs
                    lines[i+1].remove(0, 1);
                lines[i] += lines[i+1];
                lines.removeAt(i+1);
            }
    }
}

void QuotedPrintable::mergeLines(QString &line)
{
    line.replace("=\r\n", ""); // TODO check on linux
}

void QuotedPrintable::checkSoftBreak(QString& buf, QString& lBuf, int prefixLen, int addSize, bool lastChar)
{
    // If port to C++11, it must be lambda...
    // Rule 5 (RFC 2045). Soft Line Breaks
    int limit = MAX_QUOTED_PRINTABLE_LEN;
    if (buf.isEmpty()) // first sub-line
        limit -= prefixLen;
    if (!lastChar)
        limit--;
    if (lBuf.length()>limit-addSize) {
        if (!buf.isEmpty())
            buf += "=\x0d\n";
        buf += lBuf;
        lBuf.clear();
    }
}
