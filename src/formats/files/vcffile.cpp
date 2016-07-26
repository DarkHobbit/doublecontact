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
#include "vcffile.h"
#include <QStringList>
#include <QTextStream>

#include <QMessageBox> //===>

VCFFile::VCFFile()
    :FileFormat()
{
}

bool VCFFile::detect(const QString &url)
{
    QFile f(url);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QTextStream stream(&f);
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
    QStringList content;
    QTextStream stream(&file);
    do {
        content.push_back(stream.readLine());
    } while (!stream.atEnd());
    closeFile();
    return VCardData::importRecords(content, list, append, _errors);
}

bool VCFFile::exportRecords(const QString &url, ContactList &list)
{
    QMessageBox::information(0, "Debug", "VSF save is under construction");
    // TODO
}
