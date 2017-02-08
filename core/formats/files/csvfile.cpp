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

CSVFile::CSVFile()
    :FileFormat(), profile(0)
{
}

CSVFile::~CSVFile()
{
    if (profile)
        delete profile;
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

bool CSVFile::importRecords(const QString &url, ContactList &list, bool append)
{
    profile = new ExplayCSVProfile; //===>
    if (!profile)
        return false;
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    _errors.clear();
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
    int firstLine = profile->hasHeader() ? 1 : 0;
    if (!append)
        list.clear();
    for (int i=firstLine; i<rows.count(); i++) {
        ContactItem item;
        profile->importRecord(rows[i], item, _errors);
        item.calculateFields();
        list << item;
    }
    // Ready
    return (!list.isEmpty());
}

bool CSVFile::exportRecords(const QString &/*url*/, ContactList &/*list*/)
{
    if (!profile)
        return false;
    // TODO
    return false; //===>
}
