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

#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include <QFile>
#include "../iformat.h"

class FileFormat : public IFormat
{
public:
    FileFormat();
    virtual ~FileFormat();
    QStringList errors();
    QString fatalError();
    static void lossData(QStringList& errors, const QString& contactName,
        const QString& fieldName, bool condition);
    static void lossData(QStringList& errors, const QString& contactName,
        const QString& fieldName, const QString& field);
protected:
    QFile file;
    QStringList _errors;
    QString _fatalError;
    bool openFile(QString path, QIODevice::OpenMode mode);
    void closeFile();
};

#endif // FILEFORMAT_H
