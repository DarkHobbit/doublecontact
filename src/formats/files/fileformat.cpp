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

#include <QMessageBox>
#include <QObject>
#include "fileformat.h"

FileFormat::FileFormat()
{}

FileFormat::~FileFormat()
{}

bool FileFormat::openFile(QString path, QIODevice::OpenMode mode)
{
    file.setFileName(path);
    bool res = file.open(mode);
    if (!res) {
        QString msgTempl = (mode==QIODevice::ReadOnly)
            ? QObject::tr("Can't read file\n%1")
            : QObject::tr("Can't write file\n%1");
        QMessageBox::critical(0, QObject::tr("Error"), msgTempl.arg(path));
    }
    return res;
}

void FileFormat::closeFile()
{
    if (file.isOpen())
        file.close();
}
