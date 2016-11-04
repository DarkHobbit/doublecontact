/* Double Contact
 *
 * Module: VCard directory export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "vcfdirectory.h"
#include <QDir>
#include <QStringList>
#include <QTextStream>

#include "globals.h"
#include "../common/vcarddata.h"

VCFDirectory::VCFDirectory()
    :FileFormat()
{
}

VCFDirectory::~VCFDirectory()
{
}

bool VCFDirectory::importRecords(const QString &url, ContactList &list, bool append)
{
    if (!append) list.clear(); // VCardData::importRecords must be called with append=true
    QDir d(url, "*.vcf", QDir::Name | QDir::IgnoreCase, QDir::Files);
    if (!d.exists()) {
        _fatalError =  QObject::tr("Directory not exists:\n%1").arg(url);
        return false;
    }
    QStringList entries = d.entryList(QStringList("*.vcf"), QDir::Files, QDir::Name | QDir::IgnoreCase);
    if (entries.isEmpty()) {
        _fatalError =  QObject::tr("Directory not contains VCF files:\n%1").arg(url);
        return false;
    }
    VCardData data;
    _errors.clear();
    foreach (const QString& fileName, entries) {
        if (!openFile(url + QDir::separator() + fileName, QIODevice::ReadOnly))
            return false;
        QStringList content;
        QTextStream stream(&file);
        do {
            content.push_back(stream.readLine());
        } while (!stream.atEnd());
        closeFile();
        // Append one contact to list!
        data.importRecords(content, list, true, _errors);
    }
    return true;
}

bool VCFDirectory::exportRecords(const QString &url, ContactList &list)
{
    QDir d;
    d.rmpath(url);
    if (!d.mkpath(url)) {
        _fatalError = QObject::tr("Can't create directory\n%1").arg(url);
        return false;
    }
    int i = 1;
    VCardData data;
    foreach(const ContactItem& item, list) {
        // TODO use id, if present, in filename?
        QString fileName = url + QDir::separator() + QString("%1.vcf").arg((uint)i, 4, 10, QChar('0'));
        QStringList content;
        data.exportRecord(content, item);
        if (!openFile(fileName, QIODevice::WriteOnly))
            return false;
        QTextStream stream(&file);
        foreach (const QString line, content)
            stream << line << (char)13 << endl;
        closeFile();
        i++;
    }
    return true;
}
