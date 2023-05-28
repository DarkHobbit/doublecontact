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

bool VMessageData::importRecords(const BStringList &lines, DecodedMessageList &list, bool append, QStringList &errors)
{
    bool recordOpened = false;
    DecodedMessage msg;
    msg.clear();
    if (!append)
        list.clear();
    // Collect records
    for (int line=0; line<lines.count(); line++) {
        BString s = lines[line];
        if (s.isEmpty()) // vmg can contain empty lines
            continue;
        BString us = s.toUpper();
        if (us.startsWith("BEGIN:VMSG")) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
            recordOpened = true;
            msg.clear();
            msg.sources = useVMessage;
        }
        else if (us.startsWith("END:VMSG")) {
            recordOpened = false;
            //msg.calculateFields();
            list.addOrMerge(msg);
            msg.clear();
        }
        else {
            // Split type:value
            int scPos = s.indexOf(":");
            if (scPos==-1) {
                errors << S_UNKNOWN_MSG_TAG.arg(QString(s));
                continue;
            }
            QString tag = s.left(scPos).toUpper();
            QString val = s.mid(scPos+1);
            QString uVal = val.toUpper();
            BStringList ss;
            // Known tags
            if (tag=="VERSION")
                msg.version = val;
            else if (tag=="X-IRMC-STATUS") {
                if (uVal=="READ")
                    msg.status = DecodedMessage::Read;
                else if (uVal=="UNREAD")
                    msg.status = DecodedMessage::Unread;
                else {
                    errors << S_UNKNOWN_MSG_VAL.arg(QString(s));
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
                    errors << S_UNKNOWN_MSG_VAL.arg(QString(s));
            }
            else if (tag=="X-NOK-DT")
                msg.when = DateItem::readISOExtDateTimeWithZone(val);
            else if (tag=="X-MESSAGE-TYPE") {
                if (uVal=="DELIVER")
                    msg.delivered = true;
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(QString(s));
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
                    if (!s.toUpper().startsWith("DATE:"))
                        ss << s;
                }
                msg.text = decodeValue(ss.joinByLines(), errors);
            }
            else if (s!="BEGIN:VENV" && s!="END:VENV")
                errors << S_UNKNOWN_MSG_TAG.arg(QString(s));
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
bool VMessageData::importMPBRecords(const BStringList &lines, DecodedMessageList &list, bool append, QStringList &errors, bool fromArchive)
{
    bool recordOpened = false;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // non-standart types also may be non-latin
    DecodedMessage msg;
    msg.clear();
    if (!append)
        list.clear();
    BStringList mLines = lines;
    QuotedPrintable::mergeLinesets(mLines);
    bool wasBody = false; // to process multiline message
    // Collect records
    for (int line=0; line<mLines.count(); line++) {
        BString s = mLines[line];
        if (s.isEmpty()) // vmg can contain empty lines
            continue;
        BString us = s.toUpper();
        if (us.startsWith("BEGIN:VMESSAGE")) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
            recordOpened = true;
            msg.clear();
            msg.sources = fromArchive ? useVMessageArchive : useVMessage;
        }
        else if (us.startsWith("END:VMESSAGE")) {
            recordOpened = false;
            //msg.calculateFields();
            list.addOrMerge(msg);
            msg.clear();
        }
        else {
            // Split type:value
            int scPos = s.indexOf(":");
            if (scPos==-1) {
                if (wasBody)
                    msg.text +='\n' + decodeValue(s, errors);
                else
                    errors << S_UNKNOWN_MSG_TAG.arg(QString(s));
                continue;
            }
            BStringList vType = splitBySC(s.left(scPos));
            const BString tag = vType[0].toUpper();
            BString val = s.mid(scPos+1);
            BString uVal = val.toUpper();
            // Encoding & charset
            QString encoding = "";
            QString charSet = "";
            for (int i=1; i<vType.count(); i++) {
                BString typeBegin = vType[i].toUpper();
                if (typeBegin.startsWith("ENCODING="))
                    encoding = typeBegin.mid(QString("ENCODING=").length()).toUpper();
                else if (typeBegin.startsWith("CHARSET=")) {
                    charSet = typeBegin.mid(QString("CHARSET=").length()).toUpper();
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
                    errors << S_UNKNOWN_MSG_VAL.arg(QString(s));
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
                    errors << S_UNKNOWN_MSG_VAL.arg(QString(s));
            }
            else if (tag=="READ") {
                if (uVal=="1")
                    msg.status = DecodedMessage::Read;
                else if (uVal=="0")
                    msg.status = DecodedMessage::Unread;
                else
                    errors << S_UNKNOWN_MSG_VAL.arg(QString(s));
            }
            else if (tag=="DATE")
                msg.when = DateItem::readISOExtDateTimeWithZone(val);
            else if (tag=="NUMBER") {
                msg.contacts << ContactItem();
                msg.contacts.first().phones << Phone(val);
            }
            else if (tag=="BODY")
                msg.text = decodeValue(val, errors);
            else if (tag=="SUBFOLDER")
                msg.subFolder = readMPBMsgSubfolder(val);
            else if (tag=="X-IRMC-LUID")
                msg.id = val;
            else if (tag=="SMIL") {
                QString sSmil = decodeValue(val, errors);
                msg.mmsFiles << InnerFile("", "smil.smil", QDateTime(), sSmil.toLocal8Bit());
            }
            else if (tag=="ATT") {
                msg.mmsFiles << InnerFile("", "", msg.when, QByteArray());
                InnerFile& att = msg.mmsFiles.last();
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
                if (encoding=="QUOTED-PRINTABLE") {
                    // This is a case of text files with explicit-defined codec
                    // TODO think, use here toLocal8Bit() or toUtf8
                    // Or!!! use here short form of QuotedPrintable::decode without codec
                    // and preserve charset in separate attribute of InnerFile
                    // and use it when show file
                    // and ask when save all...
                    att.content = QuotedPrintable::decode(val, codec).toLocal8Bit();
                }
                else if (encoding=="BASE64" || encoding=="B")
                    att.content = QByteArray::fromBase64(val);
                else {
                    if (!encoding.isEmpty())
                        errors << S_UNKNOWN_ENCODING.arg(encoding);
                    // Encoding not defined (in my examples) already is a text with charset.
                    // TODO TEST THIS EXAMPLE! (S prazdnikon)
                    // TODO think as above
                    if (!charSet.isEmpty())
                        att.content = codec->toUnicode(val).toLocal8Bit();
                    else
                        att.content = val;

                }
                if (fileSize>0 && fileSize!=att.content.size())
                    errors << QObject::tr("File %1 has size %2, declared %3")
                        .arg(att.name).arg(att.content.size()).arg(fileSize);
            }
            else {
                if (wasBody)
                    msg.text +='\n' + decodeValue(s, errors);
                else
                    errors << S_UNKNOWN_MSG_TAG.arg(QString(s));
                continue;
            }
            wasBody = (tag=="BODY");
        }
    }
    if (recordOpened) {
        list.addOrMerge(msg);
        errors << S_LAST_SECTION;
    }
    // Ready
    return (!list.isEmpty());
}

QString VMessageData::readMPBMsgSubfolder(const BString &src)
{
    const BString bckLabel = "Backup ";
    if (src.startsWith(bckLabel))
        return QObject::tr("Backup %1", "SMS archive date")
            .arg(QString(src).mid(bckLabel.length()));
    else return "";
    // Second case: In older MPB versions here is ANSI-encoded label without date.
    // Encodind depends by country, cannot be autodetected and label is useless.
}
