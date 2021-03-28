/* Double Contact
 *
 * Module: IrMC vMessage data import
 *
 * Copyright 2018 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include <QTextCodec>
#include "quotedprintable.h"
#include "vmessagedata.h"
#include "vcarddata.h"
#include "globals.h"

#define S_UNKNOWN_ATT_SUBTYPE QObject::tr("Unknown ATT subtype at line %1: %2")

VMessageData::VMessageData()
{
}

bool VMessageData::importRecords(const QStringList &lines, DecodedMessageList &list, bool append, QStringList &errors)
{
    bool recordOpened = false;
    DecodedMessage msg;
    msg.clear();
    if (!append)
        list.clear();
    // Collect records
    for (int line=0; line<lines.count(); line++) {
        QString s = lines[line];
        if (s.isEmpty()) // vmg can contain empty lines
            continue;
        if (s.startsWith("BEGIN:VMSG", Qt::CaseInsensitive)) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
            recordOpened = true;
            msg.clear();
            msg.sources = useVMessage;
        }
        else if (s.startsWith("END:VMSG", Qt::CaseInsensitive)) {
            recordOpened = false;
            //msg.calculateFields();
            list.addOrMerge(msg);
            msg.clear();
        }
        else {
            // Split type:value
            int scPos = s.indexOf(":");
            if (scPos==-1) {
                errors << S_UNKNOWN_MSG_TAG.arg(s);
                continue;
            }
            QString tag = s.left(scPos).toUpper();
            QString val = s.mid(scPos+1);
            QString uVal = val.toUpper();
            QStringList ss;
            // Known tags
            if (tag=="VERSION")
                msg.version = val;
            else if (tag=="X-IRMC-STATUS") {
                if (uVal=="READ")
                    msg.status = DecodedMessage::Read;
                else if (uVal=="UNREAD")
                    msg.status = DecodedMessage::Unread;
                else {
                    errors << S_UNKNOWN_MSG_VAL.arg(s);
                }
            }
            else if (tag=="X-IRMC-BOX") {
                if (uVal=="INBOX")
                    msg.box = DecodedMessage::Inbox;
                else if (uVal=="OUTBOX")
                    msg.box = DecodedMessage::Outbox;
                else if (uVal=="SENTBOX")
                    msg.box = DecodedMessage::Sentbox;
                else if (uVal=="DRAFT")
                    msg.box = DecodedMessage::Draft;
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(s);
            }
            else if (tag=="X-NOK-DT")
                msg.when = DateItem::readISOExtDateTimeWithZone(val);
            else if (tag=="X-MESSAGE-TYPE") {
                if (uVal=="DELIVER")
                    msg.delivered = true;
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(s);
            }
            else if (s=="BEGIN:VCARD") { // inner contact(s)
                ss.clear();
                do {
                    ss << s;
                    s = lines[++line];
                } while (line<lines.count() && s!="END:VCARD");
                if (line<lines.count())
                    ss << s;
                VCardData vCard;
                vCard.importRecords(ss, msg.contacts, true, errors);
            }
            else if (s=="BEGIN:VBODY") { // message text
                ss.clear();
                while (line<lines.count()-1) {
                    s = lines[++line];
                    if (s=="END:VBODY") break;
                    if (!s.startsWith("Date:", Qt::CaseInsensitive))
                        ss << s;
                }
                msg.text = ss.join("\n");
            }
            else if (s!="BEGIN:VENV" && s!="END:VENV")
                errors << S_UNKNOWN_MSG_TAG.arg(s);
        }
    }
    if (recordOpened) {
        list.addOrMerge(msg);
        errors << S_LAST_SECTION;
    }
    // Ready
    return (!list.isEmpty());
}

// Format used in MPB files. It's very similar to vMessage, but tag names are different
bool VMessageData::importMPBRecords(const QStringList &lines, DecodedMessageList &list, bool append, QStringList &errors, bool fromArchive)
{
    bool recordOpened = false;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // non-standart types also may be non-latin
    DecodedMessage msg;
    msg.clear();
    if (!append)
        list.clear();
    QStringList mLines = lines;
    QuotedPrintable::mergeLinesets(mLines);
    // Collect records
    for (int line=0; line<mLines.count(); line++) {
        QString s = mLines[line];
        if (s.isEmpty()) // vmg can contain empty lines
            continue;
        if (s.startsWith("BEGIN:VMESSAGE", Qt::CaseInsensitive)) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
            recordOpened = true;
            msg.clear();
            msg.sources = fromArchive ? useVMessageArchive : useVMessage;
        }
        else if (s.startsWith("END:VMESSAGE", Qt::CaseInsensitive)) {
            recordOpened = false;
            //msg.calculateFields();
            list.addOrMerge(msg);
            msg.clear();
        }
        else {
            // Split type:value
            int scPos = s.indexOf(":");
            if (scPos==-1) {
                errors << S_UNKNOWN_MSG_TAG.arg(s);
                continue;
            }
            QStringList vType = VCardData::splitBySC(s.left(scPos));
            const QString tag = vType[0].toUpper();
            QString val = s.mid(scPos+1);
            QString uVal = val.toUpper();
            // Encoding & charset
            QString encoding = "";
            QString charSet = "";
            for (int i=1; i<vType.count(); i++) {
                if (vType[i].startsWith("ENCODING=", Qt::CaseInsensitive))
                    encoding = vType[i].mid(QString("ENCODING=").length()).toUpper();
                else if (vType[i].startsWith("CHARSET=", Qt::CaseInsensitive)) {
                    charSet = vType[i].mid(QString("CHARSET=").length()).toUpper();
                    if (charSet!=codec->name().toUpper()) {
                        codec = QTextCodec::codecForName(charSet.toLatin1());
                        if (!codec) {
                            errors << S_UNKNOWN_CHARSET.arg(charSet);
                            codec = QTextCodec::codecForName("UTF-8");
                        }
                    }
                }
            }
            // Known tags
            if (tag=="TYP") {
                if (uVal=="SMS;IN")
                    msg.box = DecodedMessage::Inbox;
                else if (uVal=="SMS;OUT")
                    msg.box = DecodedMessage::Sentbox;
                else if (uVal=="MMS;IN") {
                    msg.box = DecodedMessage::Inbox;
                    msg.isMMS = true;
                }
                else if (uVal=="MMS;OUT") {
                    msg.box = DecodedMessage::Sentbox;
                    msg.isMMS = true;
                }
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(s);
            }
            else if (tag=="BOX") {
                if (uVal=="INBOX")
                    msg.box = DecodedMessage::Inbox;
                else if (uVal=="SENT")
                    msg.box = DecodedMessage::Sentbox;
                else if (uVal=="DRAFT" || uVal=="DRAFTS")
                    msg.box = DecodedMessage::Draft;
                // TODO other foldertypes
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(s);
            }
            else if (tag=="READ") {
                if (uVal=="1")
                    msg.status = DecodedMessage::Read;
                else if (uVal=="0")
                    msg.status = DecodedMessage::Unread;
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(s);
            }
            else if (tag=="DATE")
                msg.when = DateItem::readISOExtDateTimeWithZone(val);
            else if (tag=="NUMBER") {
                msg.contacts << ContactItem();
                msg.contacts.first().phones << Phone(val);
            }
            else if (tag=="BODY") {
                if (encoding=="QUOTED-PRINTABLE")
                    msg.text = QuotedPrintable::decode(val, codec);
                else
                    msg.text = codec->toUnicode(val.toLocal8Bit());
            }
            else if (tag=="SUBFOLDER")
                msg.subFolder = val;
            else if (tag=="X-IRMC-LUID")
                msg.id = val;
            else if (tag=="SMIL") {
                QString sSmil;
                if (encoding=="QUOTED-PRINTABLE")
                    sSmil = QuotedPrintable::decode(val, codec);
                else
                    sSmil = codec->toUnicode(val.toLocal8Bit());
                msg.mmsFiles << BinarySMS();
                msg.mmsFiles.last().name = "smil.smil";
                msg.mmsFiles.last().content = sSmil.toLocal8Bit();
            }
            else if (tag=="ATT") {
                msg.mmsFiles << BinarySMS();
                BinarySMS& att = msg.mmsFiles.last();
                long fileSize = 0;
                foreach(const QString& ss, vType) {
                    if (ss=="ATT")
                        continue;
                    int eqPos = ss.indexOf("=");
                    if (eqPos == -1) {
                        errors << S_UNKNOWN_ATT_SUBTYPE.arg(line+1).arg(ss+"1");
                        continue;
                    }
                    QString varName = ss.left(eqPos).toUpper();
                    QString varValue = ss.mid(eqPos+1);
                    if (varName=="ENCODING" || varName=="CHARSET" || varName=="TYPE")
                        continue;
                    if (varName=="NAME")
                        att.name = varValue;
                    else if (varName=="SIZE")
                        fileSize = varValue.toLong();
                    else
                        errors << S_UNKNOWN_ATT_SUBTYPE.arg(line+1).arg(ss+"2");
                }
                if (encoding=="QUOTED-PRINTABLE")
                    att.content = QuotedPrintable::decode(val, codec).toLocal8Bit();
                else if (encoding=="BASE64" || encoding=="B")
                    att.content = QByteArray::fromBase64(val.toLatin1());
                else {
                    if (!encoding.isEmpty())
                        errors << S_UNKNOWN_ENCODING.arg(encoding);
                    if (!charSet.isEmpty())
                        att.content = codec->toUnicode(val.toLocal8Bit()).toLatin1();
                    else
                        att.content = val.toLocal8Bit();

                }
                if (fileSize>0 && fileSize!=att.content.size())
                    errors << QObject::tr("File %1 has size %2, declared %3")
                        .arg(att.name).arg(att.content.size()).arg(fileSize);
            }
        }
    }
    if (recordOpened) {
        list.addOrMerge(msg);
        errors << S_LAST_SECTION;
    }
    // Ready
    return (!list.isEmpty());
}
