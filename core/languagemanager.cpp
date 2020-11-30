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

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QRegExp>
#include <QTextCodec>
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
        if (fields.count()<3)
            break;
        iso639codes.insert(fields.at(2), fields.at(0));
        nativeNamesByEnglish.insert(fields.at(1), fields.at(2));
    }
    return true;
}

bool LanguageManager::loadCodecs(const QString& language, bool& qtOk)
{
    QString langCode = languageManager.nativeNameToCode(language);
    QString langPath = LanguageManager::transPath()+QDir::separator()+QString("doublecontact_%1.qm").arg(langCode);
    qtOk = false;
    bool res = tr.load(langPath);
    if (res) {
        qApp->installTranslator(&tr);
        // Qt standard translation
        QString stPath = LanguageManager::transPath()+QDir::separator()+QString("qt_%1.qm").arg(langCode);
        // File can be **.qm or **_**.qm
        if (!QFile::exists(stPath))
            stPath = LanguageManager::transPath()+QDir::separator()+QString("qt_%1.qm").arg(langCode.left(2));
        qtOk = trQt.load(stPath);
        if (qtOk)
            qApp->installTranslator(&trQt);
    }
    return res;
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

QString LanguageManager::systemLanguageNativeName()
{
    QString englishName = QLocale::system().languageToString(QLocale::system().language());
    // In Qt 4.8 we can use QLocale::system().nativeLanguageName(), not today :)
    if (nativeNamesByEnglish.contains(englishName))
            return nativeNamesByEnglish[englishName];
    else
        return "English (United Kingdom)";
}

QString LanguageManager::transPath()
{
#ifdef WIN32
    return qApp->applicationDirPath();
#else
    if (QDir("/usr/share/doublecontact/translations").exists())
        // Standard case
        return "/usr/share/doublecontact/translations";       
    else
        // Developer case
        return qApp->applicationDirPath()+"/../translations";
#endif
    // TODO what for mac?
}

QStringList LanguageManager::availableCodecs()
{
    QStringList codecs;
    foreach (const QByteArray& c, QTextCodec::availableCodecs()) {
        codecs << c;
    }
    codecs.sort();
    int uInd = codecs.indexOf("UTF-8"); // move UTF-8 to begin
    if (uInd!=-1)
        codecs.move(uInd, 0);
    uInd = codecs.indexOf("UTF-16");
    if (uInd!=-1)
        codecs.move(uInd, 1);
    return codecs;
}

LanguageManager languageManager;
