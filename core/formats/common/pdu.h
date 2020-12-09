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

#ifndef PDU_H
#define PDU_H

#include <math.h>
#include <QDataStream>
#include <QDateTime>
#include <QString>
#include "../../decodedmessagelist.h"

class PDU
{
public:
    PDU();
    static bool parseMessage(QDataStream& ds, DecodedMessage& msg, int offset, int& MsgType);
private:
    static QString readPhoneNumber(QDataStream& s, bool lenInBytes = false);
    static QDateTime readDateTime(QDataStream& s);
    static void decodeMessageBody(bool udhi, bool ucs2, QDataStream& s, DecodedMessage& msg);
    static void decodeMessageText(bool udhi, bool ucs2, int len, quint8* buff, DecodedMessage& msg);
    static quint8* convert8to7(quint8* raw, int len, int inLen, int& outLen);
    static QString decode7bit(quint8* source, int length, int inLength);
    static quint8 readInvertDecimalByte(QDataStream& s);
};

#endif // PDU_H
