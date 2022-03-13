/* Double Contact
 *
 * Module: Inner Filesystem Model
 *
 * Copyright 2022 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef INNERFILEMODEL_H
#define INNERFILEMODEL_H

#include <QAbstractTableModel>
#include <QString>

#include "contactlist.h"

enum InnerFileColumn {
    ifcFolder,
    ifcName,
    ifcTime,
    ifcSize,
    ifcLast
};

class InnerFileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit InnerFileModel(QObject *parent, ContactList* src);
    void update();
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // Helpers
    int totalSize();
    InnerFile& item(int index);
    bool saveAll(const QString &dirPath, QString &fatalError);
private:
    ContactList* _src;
    QStringList headers;
};

#endif // INNERFILEMODEL_H
