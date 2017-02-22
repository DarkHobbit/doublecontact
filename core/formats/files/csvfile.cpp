/* Double Contact
 *
 * Module: Comma-separated values (CSV) file export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include <QStringList>
#include <QTextStream>

#include "csvfile.h"

#include "../profiles/explaybm50profile.h" //===>

CSVFile::CSVFile()
    :FileFormat(), currentProfile(0)
{
    profiles << new ExplayBM50Profile;
}

CSVFile::~CSVFile()
{
    foreach (CSVProfileBase* _profile, profiles)
        delete _profile;
}

bool CSVFile::detect(const QString &url)
{
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QTextStream stream(&f);
    QString s1 = stream.readLine();
    bool res = s1.contains(","); // TODO bad method. M.b. make CSVProfile::detect and call for all profiles
    f.close();
    return res;
}

QStringList CSVFile::supportedExtensions()
{
    return (QStringList() << "csv" << "CSV");
}

QStringList CSVFile::supportedFilters()
{
    return (QStringList() << "CSV (*.csv *.CSV)");
}

QStringList CSVFile::availableProfiles()
{
    QStringList ap;
    foreach (CSVProfileBase* _profile, profiles)
        ap << _profile->name();
    return ap;
}

bool CSVFile::setProfile(const QString &name)
{
    currentProfile = 0;
    foreach (CSVProfileBase* _profile, profiles)
        if (_profile->name()==name){
            currentProfile = _profile;
            return true;
        }
    return false;
}

QString CSVFile::profile()
{
    if (currentProfile)
        return currentProfile->name();
    else
        return "";
}

bool CSVFile::importRecords(const QString &url, ContactList &list, bool append)
{
    if (!currentProfile)
        return false;
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    _errors.clear();
    _errors << "CSV support is very experimental, you can loss your data"; //===>
    QList<QStringList> rows;
    QTextStream stream(&file);
    do {
        QString line = stream.readLine();
        bool inQuotes = false;
        QString val = "";
        QStringList row;
        foreach(const QChar& c, line.trimmed()) {
            switch (c.toLatin1()) {
            case '\"':
                inQuotes = !inQuotes;
                break;
            case ';':
            case ',':
                if (!inQuotes) {
                    row << val;
                    val.clear();
                    break;
                }
            case 0:
            default:
                val += c;
                break;
            }
        }
        if (!val.isEmpty())
            row << val;
        rows << row;
    } while (!stream.atEnd());
    closeFile();
    int firstLine = currentProfile->hasHeader() ? 1 : 0;
    if (!append)
        list.clear();
    for (int i=firstLine; i<rows.count(); i++) {
        ContactItem item;
        item.originalFormat = "CSV";
        list.originalProfile = currentProfile->name();
        currentProfile->importRecord(rows[i], item, _errors);
        item.calculateFields();
        list << item;
    }
    // Ready
    return (!list.isEmpty());
}

bool CSVFile::exportRecords(const QString &url, ContactList &list)
{
    _errors << "CSV support is very experimental, you can loss your data"; //===>
    if (!currentProfile)
        return false;
    if (!currentProfile->prepareExport(list))
        return false;
    QList<QStringList> rows;
    foreach (const ContactItem& item, list) {
        QStringList row;
        currentProfile->exportRecord(row, item, _errors);
        rows << row;
    }
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    QTextStream stream(&file);
    // Header
    if (currentProfile->hasHeader())
        stream << QString("\"%1\"").arg(currentProfile->makeHeader().join("\",\"")) << endl;
    // Items
    foreach (const QStringList& row, rows)
        stream << QString("\"%1\"").arg(row.join("\",\"")) << endl;
    closeFile();
    return true;
}
