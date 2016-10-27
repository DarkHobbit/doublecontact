/* Double Contact
 *
 * Module: Configuration manager
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QApplication>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "configmanager.h"

ConfigManager::ConfigManager()
    :settings("doublecontact", "doublecontact")
{
}

QString ConfigManager::readLanguage()
{
    return settings.value("General/Language", "").toString();
}

void ConfigManager::writeLanguage(const QString &language)
{
    settings.setValue("General/Language", language);
}

QString ConfigManager::lastContactFile()
{
    QString defaultDir =
#if QT_VERSION >= 0x050000
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
        QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation);
#endif
    return settings.value("General/LastContactFile", defaultDir).toString();
}

void ConfigManager::setLastContactFile(const QString &path)
{
    settings.setValue("General/LastContactFile", path);
}

QString ConfigManager::lastImageFile()
{
    QString defaultDir =
#if QT_VERSION >= 0x050000
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
#else
        QDesktopServices::storageLocation( QDesktopServices::PicturesLocation);
#endif
    return settings.value("General/LastImageFile", defaultDir).toString();
}

void ConfigManager::setLastImageFile(const QString &path)
{
    settings.setValue("General/LastImageFile", path);
}

bool ConfigManager::showTwoPanels()
{
    return settings.value("General/ShowTwoPanels", true).toBool();
}

void ConfigManager::setShowTwoPanels(bool value)
{
    settings.setValue("General/ShowTwoPanels", value);
}

bool ConfigManager::sortingEnabled()
{
    return settings.value("General/SortingEnabled", false).toBool();
}

void ConfigManager::setSortingEnabled(bool value)
{
    settings.setValue("General/SortingEnabled", value);

}

ConfigManager configManager;
