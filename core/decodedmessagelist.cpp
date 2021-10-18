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
#include <QDir>
#include <QMap>
#include <QTextCodec>
#include <QTextStream>
#include <qglobal.h>
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
    delivered = false;
    contacts.clear();
    when = QDateTime();
    text.clear();
    isMultiPart = false;
    partNumber = 0;
    totalParts = 0;
    isMMS = false;
    mmsSubject = "";
    mmsFiles.clear();
    contactName.clear();
}

QString DecodedMessage::contactsToString() const
{
    QStringList res;
    foreach (const ContactItem& c, contacts) {
        // Paranoidal algorithm for from/to correspondent info.
        // IrMC 1.1 shows examples with N, TEL and EMAIL vCard tags inside vMessage.
        // For other formats, such as PDU, contact always contains strongly one phonenumber.
        QString peer = "";
        if (!c.phones.isEmpty())
            peer = c.phones.first().value;
        else if (!c.emails.isEmpty())
            peer = c.emails.first().value;
        if (!c.names.isEmpty() && !peer.isEmpty() && c.formatNames()!=peer)
            peer = QString("%1 (%2)").arg(c.formatNames()).arg(peer);
        else if (!c.names.isEmpty())
            peer = c.formatNames();
        else if (peer.isEmpty()) // No phone, no email, no name
            peer = c.makeGenericName();
        res << peer;
    }
    return res.join(";");
}

bool DecodedMessage::saveMMSFiles(const QString &dirPath, QString& fatalError) const
{
    QDir d;
    if (!d.exists(dirPath)) {
        if (!d.mkpath(dirPath)) {
            fatalError = S_MKDIR_ERR.arg(dirPath);
            return false;
        }
    }
    foreach (const BinarySMS& fileInfo, mmsFiles) {
        QString fileName = dirPath+QDir::separator()+fileInfo.name;
        QFile f(fileName);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(fileInfo.content);
            f.close();
            // Set message time for attachment
#if QT_VERSION >= 0x050A00 // Qt >= 5.10
            // Setting time on-the-fly, in WriteOnly mode, not works on some platforms
            if (f.open(QIODevice::Append)) {
                f.setFileTime(this->when, QFileDevice::FileBirthTime);
                f.setFileTime(this->when, QFileDevice::FileModificationTime);
                f.close();
            }
#endif
        }
        else {
            fatalError = S_WRITE_ERR.arg(fileName);
            return false;
        }
    }
    return true;
}

DecodedMessageList::DecodedMessageList(bool mergeDuplicates, bool mergeMultiParts)
    : mergeDupCount(0), mergeMultiPartCount(0),
    _mergeDuplicates(mergeDuplicates), _mergeMultiParts(mergeMultiParts)

{
    mmsCount = 0;
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
    multiText.clear();
}

bool DecodedMessageList::toCSV(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    QTextStream ss(&f);
    ss.setCodec("UTF-8");
    ss << QObject::tr("\"Date\",\"Box\",\"From/To\",\"Number\",\"Status\",\"Text\",\"Aux\"\n");
    foreach(const DecodedMessage& msg, *this) {
        // Stricted text, without qoutes and line breaks
        // TODO m.b. implement write to vMessage (VMessageData::importRecords), it's not destructive format
        QString msgText = msg.text;
        msgText.replace("\"", "'");
        msgText.replace("\n", " ");
        msgText.replace("\r", " ");
        // Write current message
        ss << "\""    << msg.when.toString("dd.MM.yyyy hh:mm:ss")
           << "\",\"" << sMsgBox[msg.box]
           << "\",\"" << msg.contactName
           << "\",\"" << msg.contactsToString()
           << "\",\"" << messageStates(msg.status, msg.delivered)
           << "\",\"" << msgText
           << "\",\"" << (msg.isMMS ? QObject::tr("MMS") : "")
           << "\"\n";
    }
    f.close();
    return true;
}

bool DecodedMessageList::saveAllMMSFiles(const QString &dirPath, QString& fatalError) const
{
    QDir d;
    if (!d.exists(dirPath)) {
        if (!d.mkpath(dirPath)) {
            fatalError = S_MKDIR_ERR.arg(dirPath);
            return false;
        }
    }
    int index = 0;
    foreach (const DecodedMessage& msg, *this) {
        index++;
        if (!msg.isMMS)
            continue;
        // Try compose MMS directory name from its id or datetime
        QString dirName = "";
        if (!msg.id.isEmpty()) {
            int posMinus = msg.id.indexOf("-");
            int posAt = msg.id.indexOf("@");
            if (posMinus>-1 && posAt>posMinus) {
                dirName = msg.id.mid(posMinus+1, posAt-posMinus-1)+"-"+msg.id.left(posMinus); // Date and id prefix
            }
        }
        if (dirName.isEmpty()) {
            if (msg.when.isValid()) {
                dirName = msg.when.toString("yyMMddhhmmss");
                if (!msg.id.isEmpty())
                    dirName += QString("-%1").arg(msg.id);
                else
                    dirName += QString("-%1").arg(index);
            }
            else
                dirName = QString::number(index);
        }
        // Save files
        if (!msg.saveMMSFiles(dirPath + QDir::separator() + dirName, fatalError))
            return false;
    }
    return true;
}

DecodedMessageList DecodedMessageList::fromContactList(const ContactList &list, const MessageSourceFlags& flags, QStringList &errors)
{
    DecodedMessageList messages(
        flags.testFlag(mergeDuplicates), flags.testFlag(mergeMultiParts));    
    if (flags.testFlag(useVMessage))
        fromVMessageList(messages, list.extra.vmsgSMS, errors, false);
    if (flags.testFlag(useVMessageArchive))
        fromVMessageList(messages, list.extra.vmsgSMSArchive, errors, true);
    if (flags.testFlag(usePDU))
        fromPDUList(messages, list.extra.pduSMS, errors, false);
    if (flags.testFlag(usePDUArchive))
        fromPDUList(messages, list.extra.pduSMSArchive, errors, true);
    // "MMS were never supported in the MPE and there will be no support in the future because the needed API for that are missing in the Android system".
    // https://www.fjsoft.at/forum/viewtopic.php?t=29865
    if (flags.testFlag(useBinary) && !list.extra.binarySMS.isEmpty()) {
        foreach(const BinarySMS& sms, list.extra.binarySMS)
            NokiaData::ReadPredefBinMessage(sms.name, sms.content, messages, true, errors);
    }
    messages.bindToContacts(list);
    return messages;
}

QString DecodedMessageList::messageBoxes(int index) const
{
    if (index<0 || index>=sMsgBox.count())
        return "";
    else
        return sMsgBox[index];
}

QString DecodedMessageList::messageStates(int index, bool delivered) const
{
    QString s = "";
    if (index>=0 && index<sMsgStatus.count() && index!=DecodedMessage::Unknown)
        s = sMsgStatus[index];
    if (delivered)
        s += "," + QObject::tr("Dlv");
    return s;
}

void DecodedMessageList::addOrMerge(DecodedMessage &msg)
{
    if (msg.isMMS)
        mmsCount++;
    // Merge multipart messages
    if (msg.isMultiPart) {
        if (_mergeMultiParts) {
            if (msg.partNumber==1) { // First part
                multiText = msg.text;
                return;
            }
            else if (msg.partNumber<msg.totalParts) { // Middle parts
                multiText += msg.text;
                return;
            }
            else { // Last part
                msg.text = multiText + msg.text;
                mergeMultiPartCount++;
            }
        }
        else
            msg.text = QString("(%1)[%2/%3]:%4")
                    .arg(msg.id).arg(msg.partNumber).arg(msg.totalParts).arg(msg.text);
    }
    // Merge duplicates
    if (_mergeDuplicates) {
        // We assume dplicate if date, text and first correspondent phonenumber are equal
#if __GNUC__ >= 5
        for(DecodedMessage& item: *this) {
#else
        for (int i=0; i<this->count(); i++) {
            DecodedMessage& item = (*this)[i];
#endif
            if (item.when==msg.when && item.contacts.count()==1 && msg.contacts.count()==1 && item.text==msg.text) {
                const ContactItem ic = item.contacts.first();
                const ContactItem mc = msg.contacts.first();
                if (ic.phones.count()==1 && mc.phones.count()==1 && ic.phones.first().value==mc.phones.first().value) {
                    // std::cout << msg.when.toString().toLocal8Bit().data() << std::endl;
                    item.sources |= msg.sources;
                    mergeDupCount++;
                    return;
                }
            }
        }
        *this << msg;
    }
    else
        *this << msg;
}

void DecodedMessageList::fromVMessageList(DecodedMessageList &messages, const QStringList &src, QStringList &errors, bool fromArchive)
{
    VMessageData vmg;
    if (src.isEmpty()) // Crash protect
        return;
    if (src.first().startsWith("BEGIN:VMSG")) { // classic/Nokia vmessage
        foreach(const QString& s, src) {
            QStringList ss = s.split("\n");
            vmg.importRecords(ss, messages, true, errors);
        }
    }
    else // MPB vmessage
        vmg.importMPBRecords(src, messages, true, errors, fromArchive);
}

void DecodedMessageList::fromPDUList(DecodedMessageList &messages, const QStringList &src, QStringList &errors, bool fromArchive)
{
    foreach(const QString& s, src) {
        QStringList ss = s.split(",");
        if (ss.length()>1) {
            QByteArray body = QByteArray::fromHex(ss[1].toLatin1());
            int MsgType;
            DecodedMessage msg;
            msg.clear();
            msg.sources = fromArchive ? usePDUArchive : usePDU;
            QDataStream ds(body);
            // SMSC data
            quint8 byte;
            ds >> byte;
            ds.skipRawData(byte);
            // PDU data
            bool res = PDU::parseMessage(ds, msg, MsgType);
            if (!res)
                    errors << QString("Unknown message type: %1").arg(MsgType);
            if (ss[0].toInt()==1) // For multipart
                msg.box = DecodedMessage::Inbox;
            if (ss.length()>2 && !msg.when.isValid()) // For outbox and multipart
                msg.when = QDateTime::fromString(ss[2], "ddMMyyyyhhmmssv");
            messages.addOrMerge(msg);
        }
        else
            errors << QObject::tr("MPB message body missing");
    }
}

void DecodedMessageList::bindToContacts(const ContactList &list)
{
    NumberNameMap contactsByName(list);
    // Bind!
    for (int i=0; i<this->count(); i++) {
        QStringList names;
        DecodedMessage& msg = (*this)[i];
        foreach(const ContactItem& c, msg.contacts)
            names << contactsByName.nameByNumber(c.phones.first().value);
        msg.contactName = names.join(";");
    }
}
