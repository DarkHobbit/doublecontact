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

#include "bstring.h"
#include "corehelpers.h"
#include "globals.h"
#include "../common/vcarddata.h"

VCFDirectory::VCFDirectory()
    :FileFormat()
{
}

VCFDirectory::~VCFDirectory()
{
}

bool VCFDirectory::setCharSet(const QString &value)
{
    charSet = value;
    return true;
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
    data.charSet = charSet;
    _errors.clear();
    foreach (const QString& fileName, entries) {
        if (!openFile(url + QDir::separator() + fileName, QIODevice::ReadOnly))
            return false;
        BStringList content = BString(file.readAll()).splitByLines();
        closeFile();
        // Append one contact to list!
        int prevErrCount = _errors.count();
        data.importRecords(content, list, true, _errors);
        if (_errors.count()>prevErrCount)
            _errors.last() += QString(" (%1)").arg(fileName);
        if (gd.readNamesFromFileName) {
            QString contName = fileName;
            contName.remove(".vcf");
            contName.remove(".VCF");
            list.last().names.clear();
            list.last().names << contName;
            list.last().fullName = contName;
        }
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
        BStringList content;
        int prevErrCount = _errors.count();
        data.exportRecord(content, item, _errors);
        if (!openFile(fileName, QIODevice::WriteOnly))
            return false;
        file.write(content.joinByLines());
        closeFile();
        i++;
        if (_errors.count()>prevErrCount)
            _errors.last() +=    QString(" (%1)").arg(item.makeGenericName());
    }
    return true;
}
