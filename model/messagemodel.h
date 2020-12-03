/* Double Contact
 *
 * Module: SMS Visualization Model
 *
 * Copyright 2020 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractTableModel>

#include "contactlist.h"
#include "decodedmessagelist.h"

enum MessageColumn {
    mcDate,
    mcCorrespondent,
    mcFolder,
    mcFlags,
    mcText,
    mcLast
};

class MessageModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MessageModel(QObject *parent, ContactList* src);
    void update(const MessageSourceFlags& flags, QStringList& errors);
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // Helpers
    DecodedMessage& item(int index);
    int mergeDupCount();
    bool saveToCSV(const QString& path);

private:
    ContactList* _src;
    DecodedMessageList msgs;
    QStringList headers;
};

#endif // MESSAGEMODEL_H
