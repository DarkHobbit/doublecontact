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
#include <QDir>
#include <QLocale>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#include <QTextCodec>

#include "configmanager.h"
#include "globals.h"
#include "contactlist.h"

ConfigManager::ConfigManager()
    :settings(0)
{}

ConfigManager::~ConfigManager()
{
    if (settings)
        delete settings;
}

void ConfigManager::prepare()
{
    // Check if program distribution is portable
    QString portableIniPath = QDir::toNativeSeparators(qApp->applicationDirPath() + "/") + "doublecontact.ini";
    settings = new QSettings(portableIniPath, QSettings::IniFormat);
    bool isPortable = settings->value("General/IsPortable", false).toBool();
    // If no, use standard config location
    if (!isPortable) {
        delete settings;
        settings = new QSettings("DarkHobbit", "doublecontact");
        settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    }
}

void ConfigManager::setDefaults(const QString &tableFont, const QString &gridColor1, const QString &gridColor2)
{
    gd.tableFont = tableFont;
    gd.gridColor1 = gridColor1;
    gd.gridColor2 = gridColor2;
}

void ConfigManager::readConfig()
{
    if (!settings)
        return;
    // Locale
    gd.dateFormat = settings->value("Locale/DateFormat", QLocale::system().dateFormat()).toString();
    gd.timeFormat = settings->value("Locale/TimeFormat", QLocale::system().timeFormat()).toString();
    gd.useSystemDateTimeFormat = settings->value("Locale/UseSystemDateTimeFormat", true).toBool();
    updateFormats();
    // Misc
    gd.openLastFilesAtStartup = settings->value("General/OpenLastFilesAtStartup", true).toBool();
    // View
    gd.showTableGrid = settings->value("View/ShowTableGrid", false).toBool();
    gd.showLineNumbers = settings->value("View/ShowLineNumbers", false).toBool();
    gd.useTableAlternateColors = settings->value("View/UseTableAlternateColors", true).toBool();
    gd.useSystemFontsAndColors = settings->value("View/UseSystemFontsAndColors", false).toBool();
    gd.tableFont = settings->value("View/TableFont", gd.tableFont).toString();
    gd.gridColor1 = settings->value("View/GridColor1", gd.gridColor1).toString();
    gd.gridColor2 = settings->value("View/GridColor2", gd.gridColor2).toString();
    // For column view
    validColumnNames.clear();
    for (int i=0; i<ccLast; i++)
         validColumnNames << contactColumnHeaders[i];
    // Column view
    gd.columnNames.clear();
    int visibleColumnCount = settings->value("VisibleColumns/Count", 0).toInt();
    for (int i=0; i<visibleColumnCount; i++) { // Fill visible columns list
        QString columnCandidate = settings->value(QString("VisibleColumns/Column%1").arg(i+1)).toString();
        if (validColumnNames.contains(columnCandidate))
            gd.columnNames.push_back((ContactColumn)validColumnNames.indexOf(columnCandidate));
    }
    if (gd.columnNames.count()==0) { // if list is empty, set default
        gd.columnNames.push_back(ccLastName);
        gd.columnNames.push_back(ccFirstName);
        gd.columnNames.push_back(ccPhone);
    }
    // Saving
    QString sPrefVer = settings->value("Saving/PreferredVCardVersion", "2.1").toString();
    if (sPrefVer=="2.1")
        gd.preferredVCFVersion = GlobalConfig::VCF21;
    else if (sPrefVer=="3.0")
        gd.preferredVCFVersion = GlobalConfig::VCF30;
    else
        gd.preferredVCFVersion = GlobalConfig::VCF40;
    gd.useOriginalFileVersion = settings->value("Saving/UseOriginalFileVCardVersion", true).toBool();
    gd.defaultCountryRule = settings->value("Saving/DefaultCountryRule", 0).toInt();
    gd.skipTimeFromDate = settings->value("Saving/SkipTimeFromDate", false).toBool();
    gd.addXToNonStandardTypes = settings->value("Saving/AddXToNonStandardTypes", false).toBool();
    gd.replaceNLNSNames = settings->value("Saving/ReplaceNLNSNames", false).toBool();
    QString sGroupFormat = settings->value("Saving/GroupFormat", "CATEGORIES").toString();
    gd.groupFormat = (sGroupFormat=="X-GROUP-MEMBERSHIP") ? GlobalConfig::gfXGroupMembership : GlobalConfig::gfCategories;
    // Loading
    gd.defaultEmptyPhoneType = settings->value("Loading/DefaultEmptyPhoneType",
        Phone::standardTypes.translate("voice")).toString(); // many phones treat type 'voice' as 'other'
    gd.warnOnNonStandardTypes = settings->value("Loading/WarnOnNonStandardTypes", true).toBool();
    gd.readNamesFromFileName = settings->value("Loading/ReadNamesFromFileName", false).toBool();
    gd.debugSave = settings->value("Loading/DebugSave", false).toBool();
}

void ConfigManager::writeConfig()
{
    if (!settings)
        return;
    // Locale
    updateFormats();
    settings->setValue("Locale/DateFormat", gd.dateFormat);
    settings->setValue("Locale/TimeFormat", gd.timeFormat);
    settings->setValue("Locale/UseSystemDateTimeFormat", gd.useSystemDateTimeFormat);
    // Misc
    settings->setValue("General/OpenLastFilesAtStartup", gd.openLastFilesAtStartup);
    // View
    settings->setValue("View/ShowTableGrid", gd.showTableGrid);
    settings->setValue("View/ShowLineNumbers", gd.showLineNumbers);
    settings->setValue("View/UseTableAlternateColors", gd.useTableAlternateColors);
    settings->setValue("View/UseSystemFontsAndColors", gd.useSystemFontsAndColors);
    settings->setValue("View/TableFont", gd.tableFont);
    settings->setValue("View/GridColor1", gd.gridColor1);
    settings->setValue("View/GridColor2", gd.gridColor2);
    // Column view
    settings->setValue("VisibleColumns/Count", gd.columnNames.count());
    for (int i=0; i<gd.columnNames.count(); i++)
        settings->setValue(QString("VisibleColumns/Column%1").arg(i+1), contactColumnHeaders[gd.columnNames[i]]);
    // Saving
    QString sPrefVer;
    switch (gd.preferredVCFVersion) {
    case GlobalConfig::VCF21:
        sPrefVer = "2.1";
        break;
    case GlobalConfig::VCF30:
        sPrefVer = "3.0";
        break;
    default:
        sPrefVer = "4.0";
        break;
    }
    settings->setValue("Saving/PreferredVCardVersion", sPrefVer);
    settings->setValue("Saving/UseOriginalFileVCardVersion", gd.useOriginalFileVersion);
    settings->setValue("Saving/DefaultCountryRule", gd.defaultCountryRule);
    settings->setValue("Saving/SkipTimeFromDate", gd.skipTimeFromDate);
    settings->setValue("Saving/AddXToNonStandardTypes", gd.addXToNonStandardTypes);
    settings->setValue("Saving/ReplaceNLNSNames", gd.replaceNLNSNames);
    QString sGroupFormat = (gd.groupFormat==GlobalConfig::gfXGroupMembership) ? "X-GROUP-MEMBERSHIP" : "CATEGORIES";
    settings->setValue("Saving/GroupFormat", sGroupFormat);
    // Loading
    settings->setValue("Loading/DefaultEmptyPhoneType", gd.defaultEmptyPhoneType);
    settings->setValue("Loading/WarnOnNonStandardTypes", gd.warnOnNonStandardTypes);
    settings->setValue("Loading/ReadNamesFromFileName", gd.readNamesFromFileName);
    settings->setValue("Loading/DebugSave", gd.debugSave);
}

QString ConfigManager::readLanguage()
{
    if (!settings)
        return "";
    return settings->value("General/Language", "").toString();
}

void ConfigManager::writeLanguage(const QString &language)
{
    if (!settings)
        return;
    settings->setValue("General/Language", language);
}

QString ConfigManager::lastContactFile()
{
    if (!settings)
        return "";
    return settings->value("General/LastContactFile", defaultDocDir()).toString();
}

void ConfigManager::setLastContactFile(const QString &path)
{
    if (!settings)
        return;
    settings->setValue("General/LastContactFile", path);
}

QString ConfigManager::lastImageFile()
{
    if (!settings)
        return "";
    return settings->value("General/LastImageFile", defaultImageDir()).toString();
}

void ConfigManager::setLastImageFile(const QString &path)
{
    if (!settings)
        return;
    settings->setValue("General/LastImageFile", path);
}

bool ConfigManager::showTwoPanels()
{
    if (!settings)
        return false;
    return settings->value("General/ShowTwoPanels", true).toBool();
}

void ConfigManager::setShowTwoPanels(bool value)
{
    if (!settings)
        return;
    settings->setValue("General/ShowTwoPanels", value);
}

void ConfigManager::writeEditConfig(int nameCount,
    int phoneCount, const QStringList& phoneTypes,
    int emailCount, const QStringList& emailTypes,
    int imCount, const QStringList& imTypes,
    int addrCount, const QStringList& addrTypes,
    int width, int height)
{
    if (!settings)
        return;
    settings->setValue("Edit/NameCount", nameCount);

    settings->setValue("Edit/PhoneCount", phoneCount);
    settings->setValue("Edit/PhoneTypes", phoneTypes);
    settings->setValue("Edit/EmailCount", emailCount);
    settings->setValue("Edit/EmailTypes", emailTypes);
    settings->setValue("Edit/ImCount", imCount);
    settings->setValue("Edit/ImTypes", imTypes);
    settings->setValue("Edit/AddrCount", addrCount);
    settings->setValue("Edit/AddrTypes", addrTypes);

    settings->setValue("Edit/Width", width);
    settings->setValue("Edit/Height", height);
}

void ConfigManager::readEditConfig(int &nameCount,
    int &phoneCount, QStringList &phoneTypes,
    int &emailCount, QStringList &emailTypes,
    int &imCount, QStringList &imTypes,
    int &addrCount, QStringList &addrTypes,
    int &width, int &height)
{
    nameCount = settings->value("Edit/NameCount", 1).toInt();

    phoneCount = settings->value("Edit/PhoneCount", 1).toInt();
    phoneTypes = settings->value("Edit/PhoneTypes").toStringList();
    emailCount = settings->value("Edit/EmailCount", 1).toInt();
    emailTypes = settings->value("Edit/EmailTypes").toStringList();
    imCount = settings->value("Edit/ImCount", 1).toInt();
    imTypes = settings->value("Edit/ImTypes").toStringList();
    addrCount = settings->value("Edit/AddrCount", 1).toInt();
    addrTypes = settings->value("Edit/AddrTypes").toStringList();

    readEditResolution(width, height);
}

void ConfigManager::readEditResolution(int &width, int &height)
{
    width = settings->value("Edit/Width", 0).toInt();
    height = settings->value("Edit/Height", 0).toInt();
}

bool ConfigManager::sortingEnabled()
{
    if (!settings)
        return false;
    return settings->value("General/SortingEnabled", false).toBool();
}

void ConfigManager::setSortingEnabled(bool value)
{
    if (!settings)
        return;
    settings->setValue("General/SortingEnabled", value);
}

ContactList::SortType ConfigManager::hardSortType()
{
    QString t = settings->value("General/HardSortType", "ByLastName").toString();
    if (t=="BySortString")
        return ContactList::SortBySortString;
    else if (t=="ByLastName")
        return ContactList::SortByLastName;
    else if (t=="ByFirstName")
        return ContactList::SortByFirstName;
    else if (t=="ByFullName")
        return ContactList::SortByFullName;
    else if (t=="ByNick")
        return ContactList::SortByNick;
    else
        return ContactList::SortByGroup;
}

void ConfigManager::setHardSortType(ContactList::SortType sortType)
{
    QString t = "BySortString";
    if (sortType==ContactList::SortByLastName)
        t = "ByLastName";
    else if (sortType==ContactList::SortByFirstName)
        t = "ByFirstName";
    else if (sortType==ContactList::SortByFullName)
        t = "ByFullName";
    else if (sortType==ContactList::SortByNick)
        t = "ByNick";
    else if (sortType==ContactList::SortByGroup)
        t = "ByGroup";
    settings->setValue("General/HardSortType", t);
}

void ConfigManager::csvConfig(QString &profile, QString &genEncoding, QString &genSeparator)
{
    if (!settings)
        return;
    profile = settings->value("CSV/LastProfile").toString();
    genEncoding = settings->value("CSV/GenericProfileEncoding", "UTF-8").toString();
    genSeparator = settings->value("CSV/GenericProfileSeparator", ",").toString();
}

void ConfigManager::setCSVConfig(const QString &profile, const QString &genEncoding, const QString &genSeparator)
{
    if (!settings)
        return;
    settings->setValue("CSV/LastProfile", profile);
    settings->setValue("CSV/GenericProfileEncoding", genEncoding);
    settings->setValue("CSV/GenericProfileSeparator", genSeparator);
}

void ConfigManager::readMessageViewConfig(MessageSourceFlags &flags)
{
    if (!settings)
        return;
    flags = 0;
    setQFlag(flags, useVMessage, settings->value("MessageView/UseVMessage", true).toBool());
    setQFlag(flags, useVMessageArchive, settings->value("MessageView/UseVMessageArchive", true).toBool());
    setQFlag(flags, usePDU, settings->value("MessageView/UsePDU", true).toBool());
    setQFlag(flags, usePDUArchive, settings->value("MessageView/UsePDUArchive", true).toBool());
    setQFlag(flags, useBinary, settings->value("MessageView/UseBinary", true).toBool());
    setQFlag(flags, mergeDuplicates, settings->value("MessageView/MergeDuplicates", true).toBool());
    // bool mergeMultiParts; // TODO
}

void ConfigManager::writeMessageViewConfig(const MessageSourceFlags &flags)
{
    if (!settings)
        return;
    settings->setValue("MessageView/UseVMessage", flags.testFlag(useVMessage));
    settings->setValue("MessageView/UseVMessageArchive", flags.testFlag(useVMessageArchive));
    settings->setValue("MessageView/UsePDU", flags.testFlag(usePDU));
    settings->setValue("MessageView/UsePDUArchive", flags.testFlag(usePDUArchive));
    settings->setValue("MessageView/UseBinary", flags.testFlag(useBinary));
    settings->setValue("MessageView/MergeDuplicates", flags.testFlag(mergeDuplicates));
    // bool mergeMultiParts; // TODO
}

QString ConfigManager::defaultDocDir()
{
    return
        #if QT_VERSION >= 0x050000
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        #else
                QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation);
        #endif
}

QString ConfigManager::defaultImageDir()
{
    return
        #if QT_VERSION >= 0x050000
                QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        #else
                QDesktopServices::storageLocation( QDesktopServices::PicturesLocation);
        #endif
}

void ConfigManager::updateFormats()
{
    if (gd.useSystemDateTimeFormat) {
        gd.dateFormat = QLocale::system().dateFormat();
        gd.timeFormat = QLocale::system().timeFormat();
    }
}

ConfigManager configManager;
