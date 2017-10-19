/* Double Contact
 *
 * Module: Global definitions
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QtGlobal>
#include <QObject>
#include "globals.h"

// Visible columns headers
ContactColumnHeaders contactColumnHeaders;

ContactColumnHeaders::ContactColumnHeaders()
{
    fill();
}

void ContactColumnHeaders::fill()
{
    clear();
    (*this)
    << S_FIRST_NAME
    << S_LAST_NAME
    << S_MIDDLE_NAME
    << S_FULL_NAME
    << S_GENERIC_NAME
    << S_PHONE
    << S_ALL_PHONES
    << S_HOME_PHONE
    << S_WORK_PHONE
    << S_CELL_PHONE
    << S_EMAIL
    << S_BDAY
    << S_GROUP
    << S_TITLE
    << S_ORG
    << S_ADDR
    << S_NICK
    << S_URL
    << S_IM
    << S_IM_JABBER
    << S_IM_ICQ
    << S_IM_SKYPE
    << S_HAS_PHONE
    << S_HAS_EMAIL
    << S_HAS_BDAY
    << S_HAS_PHOTO
    << S_SOME_PHONES
    << S_SOME_EMAILS;
}

QString StandardTypes::translate(const QString &key, bool* isStandard) const
{
    if (this->contains(key.toLower())) {
        if (isStandard) *isStandard = true;
        return (*this)[key.toLower()];
    }
    else { // Non-standard type stored as is
        if (isStandard) *isStandard = false;
        return key;
    }
}

QString StandardTypes::unTranslate(const QString &value) const
{
    return key(value, value);
}

GlobalConfig gd;
