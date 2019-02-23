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
#include "vmessagedata.h"
#include "vcarddata.h"

#define S_UNKNOWN_MSG_TAG QObject::tr("Unknown vMessage tag: %1")
#define S_UNKNOWN_MSG_VAL QObject::tr("Unknown vMessage value: %1")

VMessageData::VMessageData()
{
}

bool VMessageData::importRecords(QStringList &lines, DecodedMessageList &list, bool append, QStringList &errors)
{
    bool recordOpened = false;
    DecodedMessage msg;
    msg.clear();
    if (!append)
        list.clear();
    // Collect records
    for (int line=0; line<lines.count(); line++) {
        QString& s = lines[line];
        if (s.isEmpty()) // vmg can contain empty lines
            continue;
        if (s.startsWith("BEGIN:VMSG", Qt::CaseInsensitive)) {
            if (recordOpened)
                errors << QObject::tr("Unclosed record before line %1").arg(line+1);
            recordOpened = true;
            msg.clear();
        }
        else if (s.startsWith("END:VMSG", Qt::CaseInsensitive)) {
            recordOpened = false;
            //msg.calculateFields();
            list.push_back(msg);
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
                msg.when = QDateTime::fromString(val, "yyyyMMddThhmmssZ");
            else if (tag=="X-MESSAGE-TYPE") {
                if (uVal!="DELIVER")
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
        list.push_back(msg);
        errors << QObject::tr("Last vMessage section not closed");
    }
    // Ready
    return (!list.isEmpty());
}
