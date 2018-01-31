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

#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QTranslator>

class LanguageManager
{
public:
    LanguageManager();
    bool load(const QString& fileName);
    bool loadCodecs(const QString& language, bool& qtOk);
    QStringList nativeNames();
    QString nativeNameToCode(const QString& name);
    QString systemLanguageNativeName();
    static QString transPath();
    static QStringList availableCodecs();
private:
    QTranslator tr, trQt;
    // Ported from Stellarium, but map is reversed
    // Native name is key, code is value
    QMap<QString, QString> iso639codes;
    // English name is key, native name is value
    QMap<QString, QString> nativeNamesByEnglish;
};

extern LanguageManager languageManager;

#endif // LANGUAGEMANAGER_H
