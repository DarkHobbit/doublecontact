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

#ifndef DECODEDMESSAGELIST_H
#define DECODEDMESSAGELIST_H

#include "contactlist.h"

struct MessageSourceFlags {
    bool useVMessageSMS;
    bool usePDUSMS;
    bool useArchive;
    // bool mergeDuplicates; // TODO
};

struct DecodedMessage {
    QString version;
    enum MsgStatus { // X-IRMC-STATUS
        Read,
        Unread,
        Unknown // if messageformat not support read status (MPB)
    } status;
    enum MsgBox { // X-IRMC-BOX
        Inbox,
        Outbox,
        Sentbox,
        Draft,
        Trash // this value not appear in known to me vmsg files
    } box;
    QString subFolder;
    // X-MESSAGE-TYPE:DELIVER ignored because I don't know other xmessagetypezz
    ContactList contacts;
    QDateTime when;
    QString text;
    void clear();
};

class DecodedMessageList : public QList<DecodedMessage>
{
public:
    DecodedMessageList();
    bool toCSV(const QString& path);
    static DecodedMessageList fromContactList(ContactList& list, const MessageSourceFlags& flags, QStringList &errors);
private:
    QStringList sMsgStatus, sMsgBox;
    QString peerInfo(const ContactItem& c, const QString& defaultValue);
    static void fromPDUList(DecodedMessageList& messages, const QStringList& src, QStringList &errors);
};

#endif // DECODEDMESSAGELIST_H
