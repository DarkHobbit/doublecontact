/* Double Contact
 *
 * Module: Nokia NBF/NBU common data parts import
 *
 * Copyright 2018 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 * Partially based on code by Petr Vilem <petrusek@seznam.cz> - see below
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <limits.h>
#include <QTextCodec>

#include "nokiadata.h"
#include "pdu.h"
#include "quotedprintable.h"

#if !defined(UINT64_MAX)
#define UINT64_MAX ULLONG_MAX
#endif

NokiaData::NokiaData()
{
}

// Read message from "predefmessages" NBF/NBU pseudofolder
// Based on NbuExplorer code by Petr Vilem (petrusek@seznam.cz)
bool NokiaData::ReadPredefBinMessage(const QString& fileName, const QByteArray &src, DecodedMessageList &list, bool append, QStringList &errors)
{
    DecodedMessage msg;
    msg.clear();
    if (!append)
        list.clear();
    QDataStream ds(src);
    int MsgType;
    ds.skipRawData(0xB0);
    bool res = PDU::parseMessage(ds, msg, MsgType);
    if (!res) {
        if (MsgType==0x0C) {
            ds.device()->seek(ds.device()->pos()-1);
            ReadMMS(ds, msg, errors);
        }
        else {
            errors << QString("Unknown message type: %1").arg(MsgType);
            return false;
        }
    }
    // If phone not found... (in NBUExplorer, only for MsgType=1)
    if (msg.contacts.isEmpty())
        msg.contacts << ContactItem();
    if (msg.contacts.first().phones.isEmpty())
        msg.contacts.first().phones << Phone();
    Phone& ph = msg.contacts.first().phones.first();
    if (MsgType!=0x0C && ph.value.isEmpty())
    {
        bool res = ds.device()->seek(0x5F);
        if (!res) {
            errors << "Seeking error";
            ph.value = "Seeking error";
        }
        // Searches unicode zero
        long zeroPos = 0;
        while (!ds.atEnd()) {
            quint16 zeroCand;
            ds >> zeroCand;
            if ((!ds.atEnd()) && zeroCand==0) {
                zeroPos = ds.device()->pos();
                break;
            }
        }
        if (zeroPos > 0x61)
        {
            int len = (int)(zeroPos - 0x5F) / 2;
            ds.device()->seek(0x5F);
            ushort* raw = new ushort[len];
            ds.readRawData((char*)raw, 2*len);
            ph.value = QString::fromUtf16(raw, len);
            delete raw;
        }
        ph.value.remove(QChar('\0'));
    }
    // If date&time not found...
    if (msg.when.isNull() && fileName.length() > 16)
    {
        QString tmp = fileName.mid(8, 8);
        bool ok;
        quint64 sec = tmp.toULongLong(&ok, 16);
        if (ok) {
            const quint64 sec_1970_1980 = 315532800;
#if QT_VERSION >= 0x040700
            msg.when = QDateTime::fromMSecsSinceEpoch((sec+sec_1970_1980)*1000);
#else
#warning Nokia message date cannot be read under Qt earlier than 4.7
            msg.when = QDateTime();
            errors << "Nokia message date cannot be read under Qt earlier than 4.7: " << QString::number(sec+sec_1970_1980);
#endif
        }
    }
    msg.sources = useBinary;
    list.addOrMerge(msg);
    return true;
}

quint8 NokiaData::getU8(QDataStream &stream)
{
    quint8 res;
    stream >> res;
    return res;
}

bool NokiaData::ReadMMS(QDataStream& ds, DecodedMessage &msg, QStringList &errors)
{
    QStringList log;
    QString s;
    bool stop = false;
    bool parseParts = false;
    int errNum = errors.count();
    msg.isMMS = true;
    msg.box = DecodedMessage::Inbox; // modified below if 0x8C occured
    QString phoneFrom, phoneTo;

    // Header
    while (!ds.atEnd()) {
        quint8 secType = getU8(ds);
        switch (secType) {
            case 0x8C: {
                quint8 mmsType = getU8(ds);
                switch(mmsType) {
                    case 128: {
                        s = "SendReq";
                        msg.box = DecodedMessage::Sentbox;
                        break;
                    }
                    case 129: s = "SendConf";
                    break;
                    case 130: s = "NotificationInd";
                    break;
                    case 131: s = "NotifyrespInd";
                    break;
                    case 132: s = "RetrieveConf";
                    break;
                    case 133: s = "AcknowledgeInd";
                    break;
                    case 134: s = "DeliveryInd";
                    break;
                    default: s = "Unknown";
                }
                log << QString("MessageType: ")+s;
                break;
            }
            case 0x8F:  {
            quint8 mmsPriority = getU8(ds);
                switch(mmsPriority) {
                    case 128: s = "Low";
                    break;
                    case 129: s = "Normal";
                    break;
                    case 130: s = "High";
                    break;
                    default: s = "Unknown";
                }
                log << QString("Priority: ")+s;
                break;
            }
            case 0x98:  {
                log << QString("TransId: ")+ReadMMSTextString(ds);
                break;
            }
            case 0x8B:  {
                msg.id = ReadMMSTextString(ds);
                log << QString("MessageId: ") + msg.id;
                break;
            }
            case 0x8D:  {
                quint8 verByte = getU8(ds);
                log << QString("MMS version: %1.%2").arg((verByte & 0x70)>>4).arg((verByte & 0x0F));
                break;
            }
            case 0x89:  {
                phoneFrom = ReadMMSEncodedString(ds);
                log << QString("From: ") + phoneFrom;
                break;
            }
            case 0x97:  {
                phoneTo = ReadMMSEncodedString(ds);
                log << QString("To: ") + phoneTo;
                break;
            }
            case 0x96:  {
                msg.mmsSubject = ReadMMSEncodedString(ds);
                log << QString("Subject: ")+msg.mmsSubject;
                break;
            }
            case 0x86:  {
                log << QString("Delivery report: ")+ReadMMSYesNo(ds, errors);
                break;
            }
            case 0x90:  {
                log << QString("Read reply: ")+ReadMMSYesNo(ds, errors);
                break;
            }
            case 0x91:  {
                log << QString("Report allowed: ")+ReadMMSYesNo(ds, errors);
                break;
            }
            case 0x94:  {
                log << QString("Sender visibility: ")+ReadMMSYesNo(ds, errors);
                break;
            }
            case 0x88:  {
                log << QString("Expiry: %1").arg(ReadMMSLongInteger(ds, errors));
                break;
            }
            case 0x85:  {
                msg.when = ReadMMSDateTime(ds, errors);
                log << QString("Date: ")+msg.when.toString("yyyyMMddThhmmss");
                break;
            }
            case 0x8A:  {
                log << QString("Message class: ")+ReadMMSMessageClass(ds);
                break;
            }
            case 0x84:  {
                int contentType = getU8(ds);
                if (contentType <= 31)
                { // Content-general-form
                    ds.device()->seek(ds.device()->pos()-1);
                    ReadMMSValueLength(ds, errors);
                    contentType = getU8(ds);
                }
                QString cts = "";
                if (contentType > 31 && contentType < 128)
                { /* Constrained-media - Extension-media*/
                    ds.device()->seek(ds.device()->pos()-1);
                    cts = ReadMMSTextString(ds);
                }
                else
                { /* Constrained-media - Short Integer*/
                    contentType = contentType & 0x7F;
                    /******************************************************************
                     * A list of content-types of a MMS message can be found here:    *
                     * http://www.wapforum.org/wina/wsp-content-type.htm              *
                     ******************************************************************/
                    switch (contentType)
                    {
                        case 0x23:
                            cts = "multipart.mixed";
                            parseParts = true;
                            break;
                        case 0x33:
                            cts = "multipart.related";
                            parseParts = true;
                            break;
                        default:
                            cts = "unknown";
                            break;
                    }
                }
                log << QString("Content type = %1").arg(cts);
                bool noparams = false;
                while (!noparams)
                {
                    int testParam = getU8(ds);
                    switch (testParam)
                    {
                        case 0x89:
                            testParam = getU8(ds);
                            if (testParam < 128)
                            {
                                ds.device()->seek(ds.device()->pos()-1);
                                log << QString("Start = ") + ReadMMSTextString(ds);
                            }
                            else
                            {
                                testParam = testParam & 0x7F;
                                log << QString("Start = 0x%1").arg(testParam, 0, 16);
                            }
                            break;
                        case 0x8A:
                            testParam = getU8(ds);
                            if (testParam < 128)
                            {
                                ds.device()->seek(ds.device()->pos()-1);
                                log << QString("Param = ") + ReadMMSTextString(ds);
                            }
                            else
                            {
                                testParam = testParam & 0x7F;
                                log << QString("Param = 0x%1").arg(testParam, 0, 16);
                            }
                            break;
                        default:
                            ds.device()->seek(ds.device()->pos()-1);
                            noparams = true;
                            break;
                    }
                }

                stop = true;
                break;
            }
            case 0x99:
            case 0x9A: {
                int test = getU8(ds);
                if (test < 0x80)
                {
                    ds.device()->seek(ds.device()->pos()-1);
                    log << QString("0x%1 = %2").arg(secType).arg(ReadMMSTextString(ds));
                }
                else
                {
                    log << QString("0x%1 = 0x%2").arg(secType).arg(test, 0, 16);
                }
                break;
            }
            default:
                errors << QObject::tr("Unknown field type: 0x%1 at %2").arg(secType, 0, 16).arg(ds.device()->pos());
        }
        if (stop) break;
    }

    // Phone
    QString phone = (msg.box==DecodedMessage::Inbox ? phoneFrom : phoneTo);
    int slPos = phone.indexOf("/TYPE"); // remove '/TYPE=PLMN' suffix
    if (slPos>-1)
        phone = phone.left(slPos);
    msg.contacts << ContactItem();
    msg.contacts.first().phones << Phone(phone);

    // Body
    if (parseParts)
    {
        int partCount = (int)ReadMMSUint(ds, errors);
        log << QString("PartCnt = %1").arg(partCount);

        for (int i = 0; i < partCount; i++)
        {
            long headlen = (long)ReadMMSUint(ds, errors);
            long datalen = (long)ReadMMSUint(ds, errors);

            long ctypepos = ds.device()->pos();

            QString ctype;

            int type = getU8(ds);
            ds.device()->seek(ds.device()->pos()-1);

            if (type <= 31)
            {
                ReadMMSValueLength(ds, errors);
                type = getU8(ds);
                ds.device()->seek(ds.device()->pos()-1);
            }

            if (type > 31 && type < 128)
            {
                ctype = ReadMMSTextString(ds);
            }
            else
            {
                switch (type)
                {
                    case 0x9E:
                        ctype = "image/jpeg";
                        break;
                    case 0x83:
                        ctype = "text/plain";
                        break;
                    case 0x87:
                        ctype = "text/x-vCard";
                        break;
                    default: ctype = "unknown";
                        break;
                }
            }

            QString fileName;

            if (headlen > 4)
                fileName = ReadMMSTextString(ds);
            else // no filename present
            {
                QString ext = "";
                if (ctype=="text/plain")
                        ext = ".txt";
                fileName = QString("part_%1%2}").arg(i+1).arg(ext);
            }
            if (ctype == "application/smil" && !fileName.toLower().endsWith(".smil"))
            {
                fileName += ".smil";
            }
            log << QString("Part %1:\n headLength = %2, dataLength = %3, ctype = %4,\n filename = %5")
                .arg(i+1).arg(headlen).arg(datalen).arg(ctype).arg(fileName);
            ds.device()->seek(ctypepos + headlen);
            msg.mmsFiles << InnerFile("", fileName, msg.when, ds.device()->read(datalen));
        }
    }

    msg.text = log.join("\n");
    return (errors.count()==errNum);
}

// Parse Text-string
// text-string = [Quote <Octet 127>] text [End-string <Octet 00>]
QString NokiaData::ReadMMSTextString(QDataStream& ds)
{
    QByteArray b;
    quint8 byte;
    while ((!ds.atEnd()) && ((byte = getU8(ds))>0)) {
        if (byte==0x7F && b.isEmpty()) // optional starting octet
            continue;
        else if (byte>0x7F || byte=='"') // filename: 205, 203...
            // b.append(QString("0x%1").arg(byte, 0, 16).toLatin1());
            continue;
        else
            b.append(byte);
    }
    return QuotedPrintable::decodeFromMime(b);
}

QString NokiaData::ReadMMSEncodedString(QDataStream &ds)
{
    quint8 valLength= getU8(ds);
    if (valLength == 01) {
        getU8(ds);
        return "";
    }
    else if (valLength <= 31)
    {
        quint8 encoding = getU8(ds);
        QString sEncoding;
        QByteArray b;
        quint8 i;
        while ((!ds.atEnd()) && ((i = getU8(ds))>0))
            b.append(i);
        switch (encoding)
        {
            case 0xEA: sEncoding = "UTF-8";
            break;
            case 0x84: sEncoding = "iso-8859-1";
            break;
            case 0x85: sEncoding = "iso-8859-2";
            break;
            case 0x86: sEncoding = "iso-8859-3";
            break;
            case 0x87: sEncoding = "iso-8859-4";
            break;
            //case 0x83: // ASCII
            default: sEncoding = "UTF-8"; // ASCII
        }
        QTextCodec* codec = QTextCodec::codecForName(sEncoding.toLocal8Bit());
        return codec->toUnicode(b);
    }
    else {
        ds.device()->seek(ds.device()->pos()-1);
        return ReadMMSTextString(ds);
    }

}

QString NokiaData::ReadMMSYesNo(QDataStream &ds, QStringList &errors)
{
    quint8 byte = getU8(ds);
    switch (byte)
    {
        case 128:
            return "Yes";
        case 129:
            return "No";
        default: {
            errors << QObject::tr("Invalid MMS Yes/No value (%1) at %2").arg(byte).arg(ds.device()->pos());
            return "Unk";
        }
    }
}

long NokiaData::ReadMMSLongInteger(QDataStream &ds, QStringList &errors)
{
    quint8 octetCount = getU8(ds);
    if (octetCount > 30) {
        errors << QObject::tr("Invalid MMS octet count (%1) at %2").arg(octetCount).arg(ds.device()->pos());
        return -1;
    }
    long res = 0;
    for (int i = 0; i < octetCount; i++)
    {
        res = res << 8;
        res += getU8(ds);
    }
    return res;
}

QDateTime NokiaData::ReadMMSDateTime(QDataStream &ds, QStringList &errors)
{
    QDateTime res = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return res.addSecs(ReadMMSLongInteger(ds, errors));
}

QString NokiaData::ReadMMSMessageClass(QDataStream &ds)
{
    int cls = getU8(ds);
    QString clss = "";
    switch (cls)
    {
        case 128:
            clss = QObject::tr("Personal", "MmsClass");
            break;
        case 129:
            clss = QObject::tr("Advertisement", "MmsClass");
            break;
        case 130:
            clss = QObject::tr("Info", "MmsClass");
            break;
        case 131:
            clss = QObject::tr("Auto", "MmsClass");
            break;
        default:
            ds.device()->seek(ds.device()->pos()-1);
            clss = ReadMMSTextString(ds);
            break;
    }
    return clss;
}

quint64 NokiaData::ReadMMSValueLength(QDataStream &ds, QStringList &errors)
{
    int i = getU8(ds);
    if (i < 31) {
        // it's a short-length
        return (uint)i;
    }
    else if (i == 31) {
        // got the quote, length is an Uint
        return ReadMMSUint(ds, errors);
    }
    else {
        errors << QObject::tr("Parse value length error (%1) at %2").arg(i).arg(ds.device()->pos());
        return -1;
    }
}

quint64 NokiaData::ReadMMSUint(QDataStream &ds, QStringList &errors)
{
    quint64 result = 0;
    quint64 i = 0;
    do {
        i = (quint64)getU8(ds);
        if (result > (UINT64_MAX << 7)) {
            errors << QObject::tr("Error reading uint64_t value: overflow at %1").arg(ds.device()->pos());
            return -1;
        }
        // Shift the current value 7 steps
        result = result << 7;
        // Remove the first bit of the byte and add it to the current value
        result |= (i & 0x7F);
    }
    while ((i & 0x80) > 0);
    return result;
}

