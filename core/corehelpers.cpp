/* Double Contact
 *
 * Module: Core Helpers
 *
 * Copyright 2021 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QStringList>
#include "corehelpers.h"

// Enum settings helper
short EnumSetting::load(QSettings *settings) const
{
    QString sVal = settings->value(section+"/"+item).toString();
    QStringList allV = possibleValues();
    for (int i=0; i<allV.count(); i++)
        if (sVal==allV[i])
            return i;
    return defaultValue;
}

void EnumSetting::save(QSettings *settings, short value) const
{
    QStringList allV = possibleValues();
    if (value<0 || value >=allV.count())
        value = defaultValue;
    settings->setValue(section+"/"+item, allV[value]);
}

QStringList EnumSetting::possibleValues() const
{
    return allValues.split(";");
}

