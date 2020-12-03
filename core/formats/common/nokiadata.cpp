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

#include "nokiadata.h"
#include "pdu.h"

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
    bool res = PDU::parseMessage(ds, msg, 0xB0, MsgType);
    if (!res) {
        if (MsgType==0x0C) {
            errors << QString("MMS detected, NOT IMPLEMENTED");
            ds.device()->seek(ds.device()->pos()-1);
            /* TODO check this on Y.Z.'s NBF with MMS :)
            try
            {
                m.Mms = new Mms(currentFileName, s, s.Length);
                m.MessageTime = m.Mms.Time;
                m.MessageText = m.Mms.ParseLog;
            }
            catch { }
            */
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

