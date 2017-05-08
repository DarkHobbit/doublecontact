/* Double Contact
 *
 * Module: Nokia NBF backup file import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include <QObject>
#include <QStringList>
#include <QTextStream>
#include "nbffile.h"
#include "quazip.h"
#include "quazipdir.h"
#include "quazipfile.h"

#define NBF_VCARD_PATH QString("predefhiddenfolder/backup/WIP/32/contacts")

NBFFile::NBFFile()
    :FileFormat()
{
}

bool NBFFile::detect(const QString &url)
{
    // Check if file is zip archive and NBF_VCARD_PATH exists in archive
    QuaZip nbf(url);
    if (!nbf.open(QuaZip::mdUnzip))
        return false;
    QuaZipDir nbd(&nbf);
    return nbd.cd(NBF_VCARD_PATH);
}

QStringList NBFFile::supportedExtensions()
{
    return (QStringList() << "nbf" << "NBF");
}

QStringList NBFFile::supportedFilters()
{
    return (QStringList() << "Nokia NBF (*.nbf *.NBF)");
}

bool NBFFile::importRecords(const QString &url, ContactList &list, bool append)
{
    QuaZip nbf(url);
    if (!nbf.open(QuaZip::mdUnzip)) {
        _fatalError = QObject::tr("Can't open file");
        return false;
    }
    QuaZipDir nbd(&nbf);
    if (!nbd.cd(NBF_VCARD_PATH)) {
        _fatalError = QObject::tr("Can't open %1 directory in archive").arg(NBF_VCARD_PATH);
        return false;
    }
    // Each contact is a single vcf in NBF_VCARD_PATH inside archive
    if (!append) list.clear(); // VCardData::importRecords must be called with append=true
    foreach (const QString& itemID, nbd.entryList()) {
        // DON'T replace / to QDir::separator(), it may not work on Windows!
        if (!nbf.setCurrentFile(NBF_VCARD_PATH + "/" + itemID)) {
            _errors << QObject::tr("Can't set %1 item as current in archive").arg(itemID);
            continue;
        }
        // Open contact pseudo-file
        QuaZipFile vcf(&nbf);
        if (!vcf.open(QIODevice::ReadOnly)) {
            _errors << QObject::tr("Can't open %1 item in archive").arg(itemID);
            continue;
        }
        QTextStream stream(&vcf);
        QStringList content;
        while (!stream.atEnd())
            content << stream.readLine();
        vcf.close();
        // Append one contact to list!
        VCardData::importRecords(content, list, true, _errors);
    }
    // TODO SMS, calls
    nbf.close();
    return true;
}

bool NBFFile::exportRecords(const QString&, ContactList&)
{
    return false;
}

