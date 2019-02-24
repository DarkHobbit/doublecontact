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
#include <QTextStream>
#include "decodedmessagelist.h"
#include "formats/common/vmessagedata.h"

void DecodedMessage::clear()
{
    version.clear();
    status = DecodedMessage::Unknown;
    box = DecodedMessage::Inbox;
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

DecodedMessageList DecodedMessageList::fromContactList(const ContactList &list, QStringList &errors)
{
    DecodedMessageList messages;
    if (!list.extra.SMS.isEmpty()) {
        VMessageData vmg;
        foreach(const QString& s, list.extra.SMS) {
            QStringList ss = s.split("\n");
            vmg.importRecords(ss, messages, true, errors);
        }
    }
    else if (!list.extra.SMSBinary.isEmpty()) {
        // TODO
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
