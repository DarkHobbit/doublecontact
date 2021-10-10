/* Double Contact
 *
 * Module: Call Visualization Model
 *
 * Copyright 2021 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QFile>
#include <QTextStream>

#include "callmodel.h"
#include "modelhelpers.h"

CallModel::CallModel(QObject *parent, ContactList *src)
    : QAbstractTableModel(parent), _src(src)
{
    headers
        << tr("Type")
        << tr("TimeStamp")
        << tr("Duration")
        << tr("Number")
        << tr("Name");
}

void CallModel::update()
{
    beginResetModel();
    endResetModel();
}

QVariant CallModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation==Qt::Horizontal))
        return headers[section];
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

int CallModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return _src->extra.calls.count();
}

int CallModel::columnCount(const QModelIndex &) const
{
    return cacLast;
}

QVariant CallModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= _src->extra.calls.count())
        return QVariant();
    const CallInfo& c = _src->extra.calls[index.row()];
    if (role==Qt::DisplayRole) {
        switch (index.column()) {
            case cacType:
                return c.typeName();
            case cacTimeStamp: {
                QDateTime when = QDateTime::fromString(c.timeStamp, "yyyyMMddThhmmss");
                if (when.isValid())
                    return when.toString("dd.MM.yyyy HH:mm:ss");
                else
                    return c.timeStamp;
        }
            case cacDuration:
                return c.duration;
            case cacNumber:
                return c.number;
            case cacName:
                return c.name;
            default: return QVariant();
        }
    }
    else if (role==SortStringRole) {
        if (index.column()==cacDuration) // Duration sorts as number, not string
            return c.duration.toInt();
        else if (index.column()==cacTimeStamp) // Dates must be sort by year, not by day
            return c.timeStamp;
        else
            return emptyItemsToEnd(data(index, Qt::DisplayRole));
    }
    return QVariant();
}

bool CallModel::saveToCSV(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    QTextStream ss(&f);
    ss.setCodec("UTF-8");
    ss << QObject::tr("\"Type\",\"Date\",\"Duration\",\"Number\",\"Name\"\n");
    foreach(const CallInfo& c, _src->extra.calls) {
        ss << "\""    << c.typeName()
           << "\",\"" << c.timeStamp
           << "\",\"" << c.duration
           << "\",\"" << c.number
           << "\",\"" << c.name
           << "\"\n";
    }
    f.close();
    return true;
}

