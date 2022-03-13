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

#include <QFile>
#include <QTextStream>

#include "innerfilemodel.h"
#include "modelhelpers.h"

InnerFileModel::InnerFileModel(QObject *parent, ContactList *src)
    : QAbstractTableModel(parent), _src(src)
{
    headers
        << tr("Folder")
        << tr("Name")
        << tr("TimeStamp")
        << tr("Size");
}

void InnerFileModel::update()
{
    beginResetModel();
    endResetModel();
}

QVariant InnerFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation==Qt::Horizontal))
        return headers[section];
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

int InnerFileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return _src->extra.files.count();
}

int InnerFileModel::columnCount(const QModelIndex &) const
{
    return ifcLast;
}

QVariant InnerFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= _src->extra.files.count())
        return QVariant();
    const InnerFile& f = _src->extra.files[index.row()];
    if (role==Qt::DisplayRole) {
        switch (index.column()) {
        case ifcFolder:
            return f.relPath;
        case ifcName:
            return f.name;
        case ifcTime:
            return f.modified.toString("dd.MM.yyyy HH:mm:ss");
        case ifcSize:
            return f.content.count();
            default: return QVariant();
        }
    }
    else if (role==SortStringRole) {
        if (index.column()==ifcTime) // Dates must be sort by year, not by day
            return f.modified;
        else
            return emptyItemsToEnd(data(index, Qt::DisplayRole));
    }
    return QVariant();
}

int InnerFileModel::totalSize()
{
    return _src->extra.files.totalSize();
}

InnerFile &InnerFileModel::item(int index)
{
    return _src->extra.files[index];
}

bool InnerFileModel::saveAll(const QString &dirPath, QString &fatalError)
{
    return _src->extra.files.saveAll(dirPath, fatalError);
}

