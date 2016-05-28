/* Double Contact
 *
 * Module: Contact book visualization model
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QtAlgorithms>
#include <QMessageBox>

#include "contactmodel.h"
#include "logwindow.h"

ContactModel::ContactModel(QObject *parent, const QString& source) :
    QAbstractTableModel(parent), _source(source), _changed(false)
{
    // Default visible columns
    visibleColumns.clear();
    visibleColumns.push_back(ccLastName);
    visibleColumns.push_back(ccFirstName);
    visibleColumns.push_back(ccPhone);
}

ContactModel::~ContactModel()
{
}

QString ContactModel::source()
{
    return _source;
}

bool ContactModel::changed()
{
    return _changed;
}

void ContactModel::setVisibleColumns(const ContactColumnList& colNames)
{
    beginResetModel();
    visibleColumns.clear();
    foreach (const ContactColumn& col, colNames)
        visibleColumns.push_back(col);
    endResetModel();
}

Qt::ItemFlags ContactModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int ContactModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

int ContactModel::columnCount(const QModelIndex &) const
{
    return visibleColumns.count();
}

QVariant ContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation==Qt::Horizontal))
        return contactColumnHeaders[visibleColumns[section]];
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
          return QVariant();
      if (index.row() >= items.count())
          return QVariant();
    const ContactItem& c = items[index.row()];
    if (role==Qt::DisplayRole) {
        // Detect column
        ContactColumn col = visibleColumns[index.column()];
        switch (col) {
            case ccFirstName:  if (c.names.count()>1) return c.names[1]; else return QVariant();
            case ccLastName: if (c.names.count()>0) return c.names[0]; else return QVariant();
            case ccFullName: return c.fullName;
            case ccGenericName: return c.visibleName; // must be calculated
            case ccPhone: return c.prefPhone;
            case ccEMail: return c.prefEmail;
            case ccLast: { return QVariant(); } // Boundary case
            default: return QVariant();
        }
    }
    else if (role==Qt::BackgroundRole) {
        switch (viewMode) {
        case ContactModel::Standard:
            return (c.unknownTags.isEmpty()) ? QVariant() : QBrush(Qt::yellow);
        case ContactModel::CompareOpposite:
        case ContactModel::CompareMain:
            return(c.pairState==ContactItem::PairNotFound ? QBrush(Qt::red) :
                (c.pairState==ContactItem::PairIdentical ? QBrush(Qt::green) :
                    (c.pairState==ContactItem::PairSimilar ? QBrush(Qt::yellow) : QVariant())));
        case ContactModel::DupSearch:
            // TODO
            break;
        }
    }
    return QVariant();
}

bool ContactModel::open(const QString& path)
{
    if (path.isEmpty()) return false;
    IFormat* format = factory.createObject(path);
    if (!format) return false;
    beginResetModel();
    format->importRecords(path, items, false);
    _source = path;
    if (!format->errors().isEmpty()) {
        LogWindow* w = new LogWindow(0);
        w->setData(path, items, format->errors());
        w->exec();
        delete w;
    }
    delete format;
    endResetModel();
    _changed = false;
    return true;
}

bool ContactModel::saveAs(const QString& path)
{
    if (path.isEmpty()) return false;
    IFormat* format = factory.createObject(path);
    if (!format) return false;
    bool res = format->exportRecords(path, items);
    if (res)
        _source = path;
    if (!format->errors().isEmpty()) {
        LogWindow* w = new LogWindow(0);
        w->setData(path, items, format->errors());
        w->exec();
        delete w;
    }
    delete format;
    _changed = false;
    return res;
}

void ContactModel::close()
{
    _changed = false;
    beginResetModel();
    _source.clear();
    items.clear();
    endResetModel();
}

void ContactModel::addRow(const ContactItem& c)
{
    beginInsertRows(QModelIndex(), 0, 0);
    items.push_back(c);
    endInsertRows();
    _changed = true;
}

ContactItem& ContactModel::beginEditRow(QModelIndex& index)
{
    return items[index.row()];
}

void ContactModel::endEditRow(QModelIndex& index)
{
    _changed = true;
    emit dataChanged(index, index.sibling(index.row(), columnCount()-1));
}

void ContactModel::copyRows(QModelIndexList& indices, ContactModel* target)
{
    foreach(QModelIndex index, indices)
        target->addRow(items[index.row()]);
}

void ContactModel::removeAnyRows(QModelIndexList& indices)
{
    qSort(indices.begin(), indices.end());
    // foreach not usable here - reverse order needed
    beginRemoveRows (QModelIndex(), 0, indices.count()-1);
    for (int i=indices.count()-1; i>=0; i--)
        items.removeAt(indices[i].row());
    endRemoveRows();
    _changed = true;
}

void ContactModel::swapNames(const QModelIndexList& indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].swapNames();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::setViewMode(ContactModel::ContactViewMode mode, ContactModel *target)
{
    beginResetModel();
    viewMode = mode;
    if (mode==ContactModel::CompareMain) {
        items.compareWith(target->itemList());
        target->setViewMode(ContactModel::CompareOpposite, 0);
    }
    endResetModel();
}

ContactList &ContactModel::itemList()
{
    return items;
}

void ContactModel::testList()
{
    ContactItem c;
    Phone ph;
    Email em;
    _source = "Test list";
    beginInsertRows(QModelIndex(), 0, 3);

    // Example with ASCII names and multy-type phone
    c.fullName = "John Doe";
    c.names.clear();
    c.names.push_back("John");
    c.names.push_back("Doe");
    c.phones.clear();
    ph.number = "+1-213-555-1234"; // example from  RFC 2426
    ph.tTypes << "work" << "pref";
    c.phones.push_back(ph);
    c.emails.clear();
    c.calculateFields();
    items.push_back(c);

    // Example with UTF8 (Russian) names and email
    c.fullName = QString::fromUtf8("Александр Попов");
    c.names.clear();
    c.names.push_back(QString::fromUtf8("Александр"));
    c.names.push_back(QString::fromUtf8("Попов"));
    c.phones.clear();
    ph.number = "+79101234567"; // example for any cellular operator
    ph.tTypes.clear();
    ph.tTypes << "cell";
    c.phones.push_back(ph);
    c.emails.clear();
    em.address = "alexpopov@spb.ru";
    em.emTypes.push_back("internet");
    c.emails.push_back(em);
    c.calculateFields();
    items.push_back(c);

    // Example with UTF8 (German) names
    c.fullName = QString::fromUtf8("Hans Köster");
    c.names.clear();
    c.names.push_back(QString::fromUtf8("Hans"));
    c.names.push_back(QString::fromUtf8("Köster"));
    c.phones.clear();
    ph.number = "233";
    ph.tTypes.clear();
    ph.tTypes << "home";
    c.phones.push_back(ph);
    ph.number = "322";
    ph.tTypes.clear();
    ph.tTypes << "work";
    c.phones.push_back(ph);
    c.emails.clear();
    c.calculateFields();
    items.push_back(c);

    // Example with 4 names and without phones
    c.fullName = QString::fromUtf8("Ernst Theodor Amadeus Hoffmann");
    c.names.
            clear();
    c.names.push_back(QString::fromUtf8("Ernst"));
    c.names.push_back(QString::fromUtf8("Theodor"));
    c.names.push_back(QString::fromUtf8("Amadeus"));
    c.names.push_back(QString::fromUtf8("Hoffmann"));
    c.phones.clear();
    c.emails.clear();
    c.calculateFields();
    items.push_back(c);

    endInsertRows();
}
