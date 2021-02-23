/* Double Contact
 *
 * Module: Nokia NBF/NBU common data parts import
 *
 * Copyright 2018 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef NOKIADATA_H
#define NOKIADATA_H

#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QStringList>
#include "../../decodedmessagelist.h"

/*
External references for MMS parser:
http://heyman.info/mmsdecoder.php
http://www.openmobilealliance.org/tech/affiliates/LicenseAgreement.asp?DocName=/wap/wap-209-mmsencapsulation-20020105-a.pdf
http://www.openmobilealliance.org/tech/affiliates/LicenseAgreement.asp?DocName=/wap/wap-230-wsp-20010705-a.pdf
http://www.wapforum.org/wina/wsp-content-type.htm
http://www.nowsms.com/discus/messages/board-topics.html
http://www.nowsms.com/discus/messages/12/554.html
http://www.nowsms.com/discus/messages/12/470.html
http://www.nowsms.com/discus/messages/12/522.html
*/

class NokiaData
{
public:
    NokiaData();
    static bool ReadPredefBinMessage(const QString& fileName, const QByteArray& src, DecodedMessageList &list, bool append, QStringList &errors);
    // TODO add other nokia message types import, if examples will be avaliable
private:
    static quint8 getU8(QDataStream& stream);
    static bool ReadMMS(QDataStream& ds, DecodedMessage &msg, QStringList &errors);
    static QString ReadMMSTextString(QDataStream& ds);
    static QString ReadMMSEncodedString(QDataStream& ds);
    static QString ReadMMSYesNo(QDataStream& ds, QStringList &errors);
    static long ReadMMSLongInteger(QDataStream& ds, QStringList &errors);
    static QDateTime ReadMMSDateTime(QDataStream& ds, QStringList &errors);
    static QString ReadMMSMessageClass(QDataStream& ds);
    static quint64 ReadMMSValueLength(QDataStream& ds, QStringList &errors);
    static quint64 ReadMMSUint(QDataStream& ds, QStringList &errors);
};

#endif // NOKIADATA_H
