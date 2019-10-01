/* Double Contact
 *
 * Module: PDU SMS import
 *
 * Copyright 2018 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 * Partially based on code by Petr Vilem <petrusek@seznam.cz> - see below
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QTextCodec>
#include "pdu.h"

PDU::PDU()
{
}

bool PDU::parseMessage(QDataStream& ds, DecodedMessage& msg, int offset, int &MsgType)
{
    quint8 byte;
    ds.skipRawData(offset);
    ds >> byte;
    int MsgHeaderFlags = byte;
    bool udhi = (MsgHeaderFlags & 0x40) > 0;
    MsgType = MsgHeaderFlags & 0x0F;
    bool ucs2;
    ContactItem c;
    switch (MsgType)
    {
    case 0:
    case 4:
        msg.box = DecodedMessage::Inbox;
        c.phones << Phone(readPhoneNumber(ds));
        ds >> byte; // 00 expected
        ds >> byte;
        ucs2 = (byte == 8);
        msg.when = readDateTime(ds);
        ds >> byte;
        msg.text = decodeMessageBody(udhi, ucs2, ds);
        break;
    case 1:
        msg.box = DecodedMessage::Outbox;
        ds >> byte;
        c.phones << Phone(readPhoneNumber(ds));
        // 358? Finland? ...Nokia???
        if (c.phones.first().value == "+35856789123456789123")
            c.phones.first().value.clear();
        ds >> byte;
        ds >> byte;
        ucs2 = (byte == 8);
        ds >> byte;
        msg.text = decodeMessageBody(udhi, ucs2, ds);
        break;
    default:
        return false;
    }
    msg.contacts << c;
    return true;
}

QString PDU::readPhoneNumber(QDataStream &s, bool lenInBytes)
{
    QString sb;
    quint8 byte;
    s >> byte;
    int len = byte;
    if (len == 0) return "";
    if (lenInBytes)
    {
        len = (len - 1) * 2;
    }
    s >> byte;
    int type = (byte & 0xF0) >> 4;
    switch (type)
    {
        case 0x08: // 1000
            //sb += "*";
            goto case0x0A;
        case 0x09: // 1001
            sb += "+";
case0x0A:
        case 0x0A: // 1010
        case 0x0B: // 1011
        case 0x0E: { // 1110
            int b;
            while (len > 0)
            {
                s >> byte;
                b = byte;
// see here StringBuilder
                sb += QString::number((quint8)(b & 0xF), 16); //
                len--;
                if (len > 0)
                {
                    sb += QString::number((quint8)((b & 0xF0) >> 4), 16);
                    len--;
                }
            }
            break;
        }
        case 0x0D: {// 1101
            int len8 = (len + 1) / 2;
            int len7 = (len * 4 / 7);
            quint8* buff = new quint8[len8];
            // s.Read(buff, 0, buff.Length); // C# 3 args???
            s.readRawData((char*)buff, len8);
            sb += decode7bit(buff, len7, len8);
            delete buff;
            break;
        }
        default:
            return "Unknown phone number format";
    }
    return sb;
}

QDateTime PDU::readDateTime(QDataStream &s)
{
    int year = readInvertDecimalByte(s);
    if (year < 80) year += 2000; else year += 1900;
    int month = readInvertDecimalByte(s);
    int day = readInvertDecimalByte(s);
    int hour = readInvertDecimalByte(s);
    int min = readInvertDecimalByte(s);
    int sec = readInvertDecimalByte(s);
    return QDateTime(QDate(year, month, day), QTime(hour, min, sec));
}

QString PDU::decodeMessageBody(bool udhi, bool ucs2, QDataStream &s)
{
    quint8 byte;
    s >> byte;
    int len = byte;
    quint8* buff;
    if (ucs2)
    {
        buff = new quint8[len];
        s.readRawData((char*)buff, len);
    }
    else
    {
        int len7 = (int)ceil(7.0 * len / 8);
        buff = new quint8[len7];
        s.readRawData((char*)buff, len7);
    }
    QString res = decodeMessageText(udhi, ucs2, len, buff);
    delete[] buff;
    return res;
}

QString PDU::decodeMessageText(bool udhi, bool ucs2, int len, quint8 *buff)
{
    QString res;
    if (udhi)
    {
        int headLength = buff[0];
        int skipChars = (headLength + 1);
        // multipart
        if (ucs2)
        {
            QTextCodec* bee = QTextCodec::codecForName("UTF-16BE"); // TODO to constr.
            res = bee->toUnicode((char*)buff+skipChars, len-skipChars);
        }
        else
        {
            skipChars = (int)ceil(skipChars * 8.0 / 7);
            res = decode7bit(buff, len, len /* ??? TODO valgrind */).mid(skipChars);
        }
        bool isMultiPart = false;
        int partNumber;
        int totalParts;
        quint8 messageNumber;
        if (headLength > 4 && buff[2] == 3 && buff[4] > 1)
        {
            isMultiPart = true;
            messageNumber = buff[3];
            totalParts = buff[4];
            partNumber = buff[5];
        }
        else if (headLength > 5 && buff[2] == 4)
        {
            isMultiPart = true;
            messageNumber = buff[4];
            totalParts = buff[5];
            partNumber = buff[6];
        }
        if (isMultiPart)
            res = QString("(%1)[%2/%3]:%4")
               .arg(messageNumber).arg(partNumber)
               .arg(totalParts).arg(res);
    }
    else
    {
        if (ucs2)
        {
            QTextCodec* bee = QTextCodec::codecForName("UTF-16BE"); // TODO to constr.
            res = bee->toUnicode((char*)buff, len);
        }
        else
            res = decode7bit(buff, len, len /* ??? TODO valgrind */);
    }
    return res;
}

quint8 *PDU::convert8to7(quint8 *raw, int len, int inLen, int& outLen)
{
    quint8* result = new quint8[2*len];
    quint8 c;
    quint8 c2 = 0;
    outLen = 0;
    for (int i = 0; i < inLen; i++)
    {
        int shift = i % 7;
        c = (quint8)(c2 + ((raw[i] & (0x7F >> shift)) << shift));
        result[outLen] = c;
        outLen++;

        if (outLen >= len) break;

        shift = 7 - shift;
        c2 = (quint8)((raw[i] & (0x7F << shift)) >> shift);
        if (shift == 1)
        {
            result[outLen] = c2;
            outLen++;
            c2 = 0;
        }
    }
    return result;
}

static quint16 GSM2Unicode[] = {
    0x0040,0x00A3,0x0024,0x00A5,0x00E8,0x00E9,0x00F9,0x00EC,
    0x00F2,0x00C7,0x000A,0x00D8,0x00F8,0x000D,0x00C5,0x00E5,
    0x0394,0x005F,0x03A6,0x0393,0x039B,0x03A9,0x03A0,0x03A8,
    0x03A3,0x0398,0x039E,0x001B,0x00C6,0x00E6,0x00DF,0x00C9,
    32,33,34,35,0x00A4,37,38,39,
    40,41,42,43,44,45,46,47,         // ()*+,-./
    48,49,50,51,52,53,54,55,         // 01234567
    56,57,58,59,60,61,62,63,         // 89:;<=>?
    0x00A1,65,66,67,68,69,70,71,     // _ABCDEFG
    72,73,74,75,76,77,78,79,         // HIJKLMNO
    80,81,82,83,84,85,86,87,         // PQRSTUVW
    88,89,90,196,214,209,220,167,    // XYZ ...
    0x00BF,97,98,99,100,101,102,103, //  abcdefg
    104,105,106,107,108,109,110,111, // hijklmno
    112,113,114,115,116,117,118,119, // pqrstuvw
    120,121,122,0x00E4,0x00F6,0x00F1,0x00FC,0x00E0  // xyz ...
};

QString PDU::decode7bit(quint8* source, int length, int inLength)
{
    int outLength;
    quint8* data = convert8to7(source, length, inLength, outLength);
    QString sb;
    bool esc = false;
    for (int i = 0; i < outLength; i++)
    {
        quint16 c = GSM2Unicode[data[i]];
        if (c == 27)
        {
            esc = true;
        }
        else if (esc)
        {
            switch (c)
            {
                case 10: c = 12; break; // FORM FEED
                case 20: c = 94; break; // CIRCUMFLEX ACCENT ^
                case 40: c = 123; break; // LEFT CURLY BRACKET {
                case 41: c = 125; break; // RIGHT CURLY BRACKET }
                case 47: c = 92; break; // REVERSE SOLIDUS (BACKSLASH)
                case 60: c = 91; break; // LEFT SQUARE BRACKET [
                case 61: c = 126; break; // TILDE ~
                case 62: c = 93; break; // RIGHT SQUARE BRACKET ]
                case 64: c = 124; break; // VERTICAL BAR |
                case 101: c = 8364; break;// EURO SIGN
            }
            sb += QChar((ushort)c);
            esc = false;
        }
        else
        {
            sb += QChar((ushort)c);
        }
    }
    delete[] data;
    return sb;
}

quint8 PDU::readInvertDecimalByte(QDataStream &s)
{
    quint8 byte;
    s >> byte;
    int b = byte;
    int result = ((b & 0xF) * 10) + ((b & 0xF0) >> 4);
    return (quint8)result;
}
