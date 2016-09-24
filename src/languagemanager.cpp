/* Double Contact
 *
 * Module: Human language manager
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QFile>
#include <QRegExp>
#include "languagemanager.h"

LanguageManager::LanguageManager()
{
}

// Ported from Stellarium, but map is reversed
// Native name is key, code is value
bool LanguageManager::load(const QString &fileName)
{
    QFile inf(fileName);
    if (!inf.open(QIODevice::ReadOnly))
        return false;
    if (!iso639codes.empty())
        iso639codes.clear();
    while (!inf.atEnd())
    {
        QString record = QString::fromUtf8(inf.readLine());
        record.remove(QRegExp("[\\n\\r]*$")); // chomp new lines
        const QStringList& fields = record.split("\t", QString::SkipEmptyParts);
        iso639codes.insert(fields.at(2), fields.at(0));
    }
    return true;
}

QStringList LanguageManager::nativeNames()
{
    QStringList res;
    foreach (const QString& name, iso639codes.keys()) {
        res.append(name);
    }
    return res;
}

QString LanguageManager::nativeNameToCode(const QString& name)
{
    if (iso639codes.contains(name))
        return iso639codes[name];
    else
        return "en_GB";
}

LanguageManager languageManager;
