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

#include <QString>

#include "globals.h"
#include "messagemodel.h"

MessageModel::MessageModel(QObject *parent, ContactList* src)
    : QAbstractTableModel(parent), _src(src), msgs(0, 0)
{
    headers
        << tr("Date")
        << tr("From/To")
        << tr("Number")
        << tr("Folder")
        << tr("Flags")
        << tr("Text");
}

void MessageModel::update(const MessageSourceFlags &flags, QStringList& errors)
{
    errors.clear();
    beginResetModel();
    msgs = DecodedMessageList::fromContactList(*_src, flags, errors);
    endResetModel();
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation==Qt::Horizontal))
        return headers[section];
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return msgs.count();
}

int MessageModel::columnCount(const QModelIndex &) const
{
    return mcLast;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= msgs.count())
        return QVariant();
    const DecodedMessage& m = msgs[index.row()];
    if (role==Qt::DisplayRole) {
        switch (index.column()) {
            case mcDate:
#if QT_VERSION >= 0x051000
                return m.when.toString("dd.MM.yyyy HH:mm:ss t");
#else
            return m.when.toString("dd.MM.yyyy HH:mm:ss");
#endif
            case mcCorrespondent:
                return m.contactName;
            case mcNumber:
                return m.contactsToString();
            case mcText:
                return (m.isMMS ? tr("<MMS>") : m.text);
            case mcFolder: {
                QString s = msgs.messageBoxes(m.box);
                if (!m.subFolder.isEmpty())
                    s += " " + m.subFolder;
                return s;
            }
            case mcFlags:
                return msgs.messageStates(m.status, m.delivered);
            case mcLast:
            default: return QVariant();
        }
    }
    else if (role==SortStringRole) {
        if (index.column()==mcDate) // Dates must be sort by year, not by day
            return m.when.toString("yyyyMMddThhmmss");
        else
            return data(index, Qt::DisplayRole);
    }
    return QVariant();
}

DecodedMessage &MessageModel::item(int index)
{
    return msgs[index];
}

int MessageModel::mmsCount()
{
    return msgs.mmsCount;
}

int MessageModel::mergeDupCount()
{
    return msgs.mergeDupCount;
}

int MessageModel::mergeMultiPartCount()
{
    return msgs.mergeMultiPartCount;
}

bool MessageModel::saveToCSV(const QString &path)
{
    return msgs.toCSV(path);
}

bool MessageModel::saveAllMMSFiles(const QString &dirPath, QString& fatalError) const
{
    return msgs.saveAllMMSFiles(dirPath, fatalError);
}
