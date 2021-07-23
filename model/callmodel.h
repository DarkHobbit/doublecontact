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
#ifndef CALLMODEL_H
#define CALLMODEL_H

#include <QAbstractTableModel>
#include <QString>

#include "contactlist.h"

enum CallColumn {
    cacType,
    cacTimeStamp,
    cacDuration,
    cacNumber,
    cacName,
    cacLast
};

class CallModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CallModel(QObject *parent, ContactList* src);
    void update();
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // Helpers
    bool saveToCSV(const QString& path);
private:
    ContactList* _src;
    QStringList headers;
};

#endif // CALLMODEL_H
