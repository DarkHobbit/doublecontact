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

#include "bstring.h"
#include "contactlist.h"
#include "decodedmessagelist.h"
#include "vdata.h"

class VMessageData: public VData
{
public:
    VMessageData();
    bool importRecords(const BStringList& lines, DecodedMessageList& list, bool append, QStringList& errors);
    bool importMPBRecords(const BStringList& lines, DecodedMessageList& list, bool append, QStringList& errors, bool fromArchive);
    static QString readMPBMsgSubfolder(const BString& src);
};

#endif // VMESSAGEDATA_H
