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

#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "contactlist.h"
#include "formats/formatfactory.h"

// Visible columns
enum ContactColumn {
  ccFirstName, ccSecondName, ccFullName, // names
  ccPhone, // first or preferred phone
  ccEMail, // first or preferred email
  ccLast
};

class ContactModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ContactModel(QObject *parent, const QString& source);
    ~ContactModel();
    QString source(); // file path or network source
    bool changed();    // has contact book unsaved changes?
    // Base model implementation methods
    Qt::ItemFlags flags(const QModelIndex &) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    // Save and open methods
    bool open(const QString& path);
    bool save();
    // Contact operation methods
    void addRow(const ContactItem& c);
    ContactItem& beginEditRow(QModelIndex& index);
    void endEditRow(QModelIndex& index);
    void copyRows(QModelIndexList& indices, ContactModel* target);
    void removeAnyRows(QModelIndexList& indices);
    void swapNames(const QModelIndexList& indices);
    // Test data
    void testList();

signals:

public slots:
private:
    QString _source; // file path or network source
    bool _changed;   // has contact book unsaved changes?
    ContactList items;
    QList<ContactColumn> visibleColumns;
    FormatFactory factory;
};

#endif // CONTACTMODEL_H
