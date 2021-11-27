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
#ifndef COREHELPERS_H
#define COREHELPERS_H

#include <QFlags>
#include <QSettings>
#include <QString>
#include <QStringList>

// Enum settings helper
struct EnumSetting {
    QString section;    // section name in config/registry
    QString item;       // config item name
    QString allValues;  // semicolon-sepatated possible values
    short defaultValue; // index of default value
    short load(QSettings* settings) const;
    void save(QSettings* settings, short value) const;
    QStringList possibleValues() const;
};

// QFlag helper
template<typename T>
void setQFlag(QFlags<T>& flags, T f, bool value)
{
    if (value)
        flags |= f;
    else
        flags &= ~f;
}

#endif // COREHELPERS_H
