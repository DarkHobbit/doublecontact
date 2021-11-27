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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QtGlobal>
#include <QSettings>
#include <QStringList>

#include "contactlist.h"
#include "corehelpers.h"
#include "decodedmessagelist.h"

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();
    void prepare();
    // Common configuration, managed by config dialog
    void setDefaults(const QString& tableFont, const QString& gridColor1, const QString& gridColor2);
    void readConfig();
    void writeConfig();
    void addFileToRemove(const QString& path);
    // Separate settings, managed by main window and other dialogs
    QString readLanguage();
    void writeLanguage(const QString& language);
    QString lastContactFile();
    void setLastContactFile(const QString& path);
    QString lastImageFile();
    void setLastImageFile(const QString& path);
    bool showTwoPanels();
    void setShowTwoPanels(bool value);
    void writeEditConfig(int nameCount,
        int phoneCount, const QStringList& phoneTypes,
        int emailCount, const QStringList& emailTypes,
        int imCount, const QStringList& imTypes,
        int addrCount, const QStringList& addrTypes,
        int width, int height);
    void readEditConfig(int& nameCount,
        int& phoneCount, QStringList& phoneTypes,
        int& emailCount, QStringList& emailTypes,
        int& imCount, QStringList& imTypes,
        int& addrCount, QStringList& addrTypes,
        int& width, int& height);
    void readEditResolution(int& width, int& height);
    void readSortConfig(const QString& tableName,
        bool& enabled, int& column, Qt::SortOrder& order);
    void writeSortConfig(const QString& tableName,
        bool enabled, int column, Qt::SortOrder order);
    ContactList::SortType hardSortType();
    void setHardSortType(ContactList::SortType sortType);
    void csvConfig(QString& profile, QString& genEncoding, QString& genSeparator);
    void setCSVConfig(const QString& profile, const QString& genEncoding, const QString& genSeparator);
    void readMessageViewConfig(MessageSourceFlags& flags);
    void writeMessageViewConfig(const MessageSourceFlags& flags);
    static QString defaultDocDir();
    static QString defaultImageDir();
    QStringList validColumnNames; // Available list
private:
    QSettings* settings;
    QStringList filesToRemove;
    void updateFormats();
};

extern ConfigManager configManager;

#endif // CONFIGMANAGER_H
