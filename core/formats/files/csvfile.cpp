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
#include "csvfile.h"
#include <QStringList>
#include <QTextStream>

// TODO csv support is very alpha

#define S_CSV_ROW_TOO_SHORT QObject::tr("CSV row too short for this profile at line %1")

CSVFile::CSVFile()
    :FileFormat()
{
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
    int firstLine = 1 ; // TODO CSVProfile::hasHeader() ? 1 : 0;
    if (!append)
        list.clear();
    for (int i=firstLine; i<rows.count(); i++) {
        // TODO profile begin - it's Explay sample (not all fields)
        QStringList& row = rows[i];
        ContactItem item;
        if (row.count()<13) {
            _errors << S_CSV_ROW_TOO_SHORT.arg(i+1);
        }
        item.fullName = row[0];
        item.names << row[3] << row[1] << row[2] << "" << row[4];
        item.dropFinalEmptyNames();
        item.title = row[5];
        item.organization = row[6];
        item.birthday.value = QDateTime::fromString(row[7]); // TODO check
        // "SIP address","Push-to-talk","Share view"
        item.id = row[11];
        if (!row[12].isEmpty())
            item.phones << Phone(row[12], "pref");
        if (row.count()>13 && !row[13].isEmpty())
            item.phones << Phone(row[13], "home");
        if (row.count()>14 && !row[14].isEmpty())
            item.phones << Phone(row[14], "fax");
        if (row.count()>15 && !row[15].isEmpty())
            item.phones << Phone(row[15], "video");
        if (row.count()>16)
            item.url = row[16];
        // TODO see other rows...
        item.calculateFields();
        list << item;
        // TODO profile end;
    }
    // Ready
    return (!list.isEmpty());
}

bool CSVFile::exportRecords(const QString &/*url*/, ContactList &/*list*/)
{
    // TODO
    return false; //===>
}
