/* Double Contact
 *
 * Module: Decodes SMS structures
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include "decodedmessagelist.h"
#include "formats/common/nokiadata.h"
#include "formats/common/pdu.h"
#include "formats/common/vmessagedata.h"

void DecodedMessage::clear()
{
    version.clear();
    status = DecodedMessage::Unknown;
    box = DecodedMessage::Inbox;
    subFolder.clear();
    contacts.clear();
    when = QDateTime();
    text.clear();
}

DecodedMessageList::DecodedMessageList()
{
    sMsgStatus
        << QObject::tr("Read")
        << QObject::tr("UnRd")
        << QObject::tr("Unk.");
    sMsgBox
        << QObject::tr("Inbox")
        << QObject::tr("Outbox")
        << QObject::tr("Sentbox")
        << QObject::tr("Draft")
        << QObject::tr("Trash");
}

bool DecodedMessageList::toCSV(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    QTextStream ss(&f);
    ss.setCodec("UTF-8");
    ss << QObject::tr("\"Date\",\"Box\",\"From/To\",\"Status\",\"Text\",\"Aux\"\n");
    foreach(const DecodedMessage& msg, *this) {
        // Paranoidal algorithm for from/to correspondent info.
        // IrMC 1.1 shows examples with N, TEL and EMAIL vCard tags inside vMessage.
        // For other formats, such as PDU, contact always contains strongly one phonenumber.
        QString peer = QObject::tr("Empty");
        if (!msg.contacts.isEmpty())
            peer = peerInfo(msg.contacts.first(), peer);
        // Additional contacts (most paranoidal vMessage case)
        QString auxPeers = "";
        for (int i=1; i<msg.contacts.count(); i++)
            auxPeers += ", " + peerInfo(msg.contacts[i], "");
        if (!auxPeers.isEmpty())
            auxPeers.remove(0, 1);
        // Stricted text, without qoutes and line breaks
        // TODO m.b. implement write to vMessage (VMessageData::importRecords), it's not destructive format
        QString msgText = msg.text;
        msgText.replace("\"", "'");
        msgText.replace("\n", " ");
        // Write current message
        ss << "\""    << msg.when.toString("dd.MM.yyyy hh:mm:ss")
           << "\",\"" << sMsgBox[msg.box]
           << "\",\"" << peer
           << "\",\"" << sMsgStatus[msg.status]
           << "\",\"" << msgText
           << "\",\"" << auxPeers << "\"\n";
    }
    f.close();
    return true;
}

DecodedMessageList DecodedMessageList::fromContactList(ContactList &list, const MessageSourceFlags& flags, QStringList &errors)
{
    DecodedMessageList messages;
    if (flags.useVMessageSMS) {
        VMessageData vmg;
        if (list.extra.vmsgSMS.contains("BEGIN:VMSG")) { // classic/Nokia vmessage
            foreach(const QString& s, list.extra.vmsgSMS) {
                QStringList ss = s.split("\n");
                vmg.importRecords(ss, messages, true, errors); // TODO merge duplicates on demand
            }
        }
        else { // MPB vmessage
            vmg.importMPBRecords(list.extra.vmsgSMS, messages, true, errors);
            if (flags.useArchive)
                vmg.importMPBRecords(list.extra.vmsgSMSArchive, messages, true, errors);
        }
    }
    if (flags.usePDUSMS) {
        fromPDUList(messages, list.extra.pduSMS, errors);
        if (flags.useArchive)
            fromPDUList(messages, list.extra.pduSMSArchive, errors);
        // "MMS were never supported in the MPE and there will be no support in the future because the needed API for that are missing in the Android system".
        // https://www.fjsoft.at/forum/viewtopic.php?t=29865
    }
    if (flags.usePDUSMS && !list.extra.binarySMS.isEmpty()) {
        foreach(const BinarySMS& sms, list.extra.binarySMS)
             // TODO merge duplicates on demand
            NokiaData::ReadPredefBinMessage(sms.name, sms.content, messages, true, errors);
    }
    return messages;
}

QString DecodedMessageList::peerInfo(const ContactItem &c, const QString& defaultValue)
{
    QString peer = defaultValue;
    if (!c.phones.isEmpty())
        peer = c.phones.first().value;
    else if (!c.emails.isEmpty())
        peer = c.emails.first().value;
    if (!c.names.isEmpty())
        peer = QString("%1 (%2)").arg(c.formatNames()).arg(peer);
    return peer;
}

void DecodedMessageList::fromPDUList(DecodedMessageList &messages, const QStringList &src, QStringList &errors)
{
    foreach(const QString& s, src) {
        QStringList ss = s.split(",");
        if (ss.length()>1) {
            QByteArray body = QByteArray::fromHex(ss[1].toLatin1());
            int MsgType;
            DecodedMessage msg;
            msg.clear();
            QDataStream ds(body);
            PDU::parseMessage(ds, msg, 1, MsgType);
            messages << msg;  // TODO merge duplicates on demand
            // Todo field 0
            if (ss.length()>2 && !messages.last().when.isValid())
                messages.last().when = QDateTime::fromString(ss[2], "ddMMyyyyhhmmssv");
        }
        else
            errors << QObject::tr("MPB message body missing");
    }
}
