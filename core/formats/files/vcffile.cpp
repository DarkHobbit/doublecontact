/* Double Contact
 *
 * Module: VCard file export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "corehelpers.h"
#include "vcffile.h"
#include <QStringList>
#include <QTextStream>

VCFFile::VCFFile()
    :FileFormat()
{
}

bool VCFFile::setCharSet(const QString &value)
{
    charSet = value;
    return true;
}

bool VCFFile::detect(const QString &url)
{
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QTextStream stream(&f); // TODO file?
    QString s1 = stream.readLine();
    bool res = s1.startsWith("BEGIN:VCARD", Qt::CaseInsensitive);
    f.close();
    return res;
}

QStringList VCFFile::supportedExtensions()
{
    return (QStringList() << "vcf" << "VCF");
}

QStringList VCFFile::supportedFilters()
{
    return (QStringList() << "vCard (*.vcf *.VCF)");
}

bool VCFFile::importRecords(const QString &url, ContactList &list, bool append)
{
    QStringList data;
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    _errors.clear();
    BStringList content = BString(file.readAll()).splitByLines();
    closeFile();
    return VCardData::importRecords(content, list, append, _errors);
}

bool VCFFile::exportRecords(const QString &url, ContactList &list)
{
    BStringList content;
    if (!VCardData::exportRecords(content, list, _errors))
        return false;
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    _errors.clear();
    file.write(content.joinByLines());
    closeFile();
    return true;
}
