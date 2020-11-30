/* Double Contact
 *
 * Module: IrMC vMessage data import
 *
 * Copyright 2019 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef VMESSAGEDATA_H
#define VMESSAGEDATA_H

#include <QStringList>
#include "../../contactlist.h"
#include "../../decodedmessagelist.h"

class VMessageData
{
public:
    VMessageData();
    bool importRecords(const QStringList& lines, DecodedMessageList& list, bool append, QStringList& errors);
    bool importMPBRecords(const QStringList& lines, DecodedMessageList& list, bool append, QStringList& errors, bool fromArchive);
};

#endif // VMESSAGEDATA_H
