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

#include <QMap>
#include <QObject>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>
#include "nbffile.h"
#include "quazip.h"
#include "quazipdir.h"
#include "quazipfile.h"
#include "../common/vcarddata.h"

#define NBF_VCARD_PATH QString("predefhiddenfolder/backup/WIP/32/contacts")
#define S_ERR_OPEN_ARCH_ITEM QObject::tr("Can't open %1 item in archive")

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
        _fatalError = S_READ_ERR.arg(url);
        return false;
    }
    QuaZipDir nbd(&nbf);
    if (!nbd.cd(NBF_VCARD_PATH)) {
        _fatalError = QObject::tr("Can't open %1 directory in archive").arg(NBF_VCARD_PATH);
        nbf.close();
        return false;
    }
    // Each contact is a single vcf in NBF_VCARD_PATH inside archive
    if (!append) list.clear(); // VCardData::importRecords must be called with append=true
    list.originalPath = url;
    foreach (const QString& itemID, nbd.entryList()) {
        // DON'T replace / to QDir::separator(), it may not work on Windows!
        if (!nbf.setCurrentFile(NBF_VCARD_PATH + "/" + itemID)) {
            _errors << QObject::tr("Can't set %1 item as current in archive").arg(itemID);
            continue;
        }
        // Open contact pseudo-file
        QuaZipFile vcf(&nbf);
        if (!vcf.open(QIODevice::ReadOnly)) {
            _errors << S_ERR_OPEN_ARCH_ITEM.arg(itemID);
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

bool NBFFile::exportRecords(const QString &url, ContactList &list)
{
    // Check old name and existing
    if (list.originalPath.isEmpty()
            || !list.originalPath.contains("NBF", Qt::CaseInsensitive))
    {
        _fatalError = QObject::tr("NBF file can be saved only if original addressbook also was in NBF format");
        return false;
    }
    QFile inZ(list.originalPath);
    if (!inZ.exists()) {
        _fatalError = QObject::tr("Original NBF file was moved or deleted");
        return false;
    }
    // Unpack original archive
    QuaZip nbf(list.originalPath);
    if (!nbf.open(QuaZip::mdUnzip)) {
        _fatalError = S_READ_ERR.arg(list.originalPath);
        return false;
    }
    // Archive properties
    bool is64 = nbf.isZip64Enabled();
    QString arComment = nbf.getComment();
    QTextCodec* fnCodec = nbf.getFileNameCodec();
    QTextCodec* commCodec = nbf.getCommentCodec();
    // Read entire archive
    QMap<QString, QByteArray> arFiles;
    QMap<QString, QuaZipFileInfo64> arInfo;
    nbf.goToFirstFile();
    do {
        QString fileName = nbf.getCurrentFileName();
        QuaZipFileInfo64 info;
        nbf.getCurrentFileInfo(&info);
        // Don't write directories. Bad method, is better?
        if (info.uncompressedSize==0)
            continue;
        // Skip if fileName is *.vcf
        if (fileName.endsWith(".vcf", Qt::CaseInsensitive))
            continue;
        QuaZipFile f(&nbf);
        if (f.open(QIODevice::ReadOnly)) {
            arFiles[fileName] = f.readAll();
            f.close();
        }
        else
            _errors << S_ERR_OPEN_ARCH_ITEM.arg(fileName);
        arInfo[fileName] = info;
    } while (nbf.goToNextFile());
    nbf.close();
    // Write modified archive
    nbf.setZipName(url);
    nbf.setZip64Enabled(is64);
    nbf.setFileNameCodec(fnCodec);
    nbf.setCommentCodec(commCodec);
    if (!nbf.open(QuaZip::mdCreate)) {
        _fatalError = S_WRITE_ERR.arg(url);
        return false;
    }
    if (!arComment.isEmpty())
        nbf.setComment(arComment);
    foreach (const QString fileName, arFiles.keys()) {
        QuaZipFile f(&nbf);
        // TODO attrs!
        if (f.open(QIODevice::WriteOnly, QuaZipNewInfo(arInfo[fileName]))) {
            f.write(arFiles[fileName]);
            f.close();
        }
        else
            _errors << S_ERR_OPEN_ARCH_ITEM.arg(fileName);
    }
    // Add vcf files
    VCardData data;
    int i = 1;
    foreach (const ContactItem& item, list) {
        QStringList lines;
        data.exportRecord(lines, item, _errors);
        QString fileName = QString("/%1.vcf").arg(i);
        QuaZipFile f(&nbf);
        if (f.open(QIODevice::WriteOnly,
                   QuaZipNewInfo(NBF_VCARD_PATH + fileName)))
        {
            QTextStream stream(&f);
            foreach (const QString line, lines)
                stream << line << (char)13 << endl;
            f.close();
        }
        else
            _errors << S_ERR_OPEN_ARCH_ITEM.arg(fileName);
        i++;
    }
    // Done
    nbf.close();
    return true;
}

