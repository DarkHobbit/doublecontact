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
#include <QStringList>
#include "../../decodedmessagelist.h"

class NokiaData
{
public:
    NokiaData();
    static bool ReadPredefBinMessage(const QString& fileName, const QByteArray& src, DecodedMessageList &list, bool append, QStringList &errors);
    // TODO add other nokia message types import, if examples will be avaliable
private:
};

#endif // NOKIADATA_H
