/* Double Contact
 *
 * Module: Abstract class for file export/import format
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

#include "fileformat.h"
#include "globals.h"

FileFormat::FileFormat()
{}

FileFormat::~FileFormat()
{}

QStringList FileFormat::errors()
{
    return _errors;
}

QString FileFormat::fatalError()
{
    return _fatalError;
}

bool FileFormat::openFile(QString path, QIODevice::OpenMode mode)
{
    file.setFileName(path);
    bool res = file.open(mode);
    if (!res)
        _fatalError = ((mode==QIODevice::ReadOnly) ? S_READ_ERR : S_WRITE_ERR).arg(path);
    return res;
}

void FileFormat::closeFile()
{
    if (file.isOpen())
        file.close();
}

void FileFormat::lossData(QStringList &errors, const QString &contactName, const QString &fieldName, bool condition)
{
    if (condition)
        errors << S_ERR_UNSUPPORTED_TAG.arg(contactName).arg(fieldName.toLower());
}

void FileFormat::lossData(QStringList &errors, const QString &contactName, const QString &fieldName, const QString &field)
{
    lossData(errors, contactName, fieldName, !field.isEmpty());
    // TODO unify UDXFile with it
}
