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
#include <QVector>

#include "contactlist.h"
#include "formats/formatfactory.h"
#include "formats/files/csvfile.h"
#include "globals.h"
#include "recentlist.h"

class ContactModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ContactModel(QObject *parent, const QString& source, RecentList& recent);
    ~ContactModel();
    QString source(); // file path or network source
    FormatType sourceType();
    bool changed();    // has contact book unsaved changes?
    void updateVisibleColumns();
    enum ContactViewMode {
        Standard,
        StandardWithQuickView,
        CompareMain,
        CompareOpposite,
        DupSearch // TODO
    };
    // Base model implementation methods
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    // Drag'n'drop implementation methods
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData (const QMimeData * data, Qt::DropAction action,
          int, int column, const QModelIndex& index);
    bool removeRows(int row, int count, const QModelIndex&);
    // Save and open methods
    bool open(const QString& path, FormatType fType, QStringList &errors, QString &fatalError);
    bool saveAs(const QString& path, FormatType fType, QStringList &errors, QString &fatalError);
    void close();
    // Contact operation methods
    void addRow(const ContactItem& c);
    ContactItem& beginEditRow(QModelIndex& index);
    void endEditRow(QModelIndex& index);
    void copyRows(QModelIndexList& indices, ContactModel* target);
    void removeAnyRows(QModelIndexList& indices);
    void swapNames(const QModelIndexList& indices);
    void splitNames(const QModelIndexList& indices);
    void joinNames(const QModelIndexList& indices);
    void dropSlashes(const QModelIndexList& indices);
    void generateFullNames(const QModelIndexList& indices);
    void parseFullName(const QModelIndexList& indices);
    void dropFullNames(const QModelIndexList& indices);
    void reverseFullNames(const QModelIndexList& indices);
    void formatPhones(const QModelIndexList& indices, const QString& templ);
    void splitNumbers(const QModelIndexList& indices);
    void intlPhonePrefix(const QModelIndexList& indices, int countryRule);
    // Group operation methods
    void addGroup(const QString& group);
    void renameGroup(const QString& oldName, const QString& newName);
    void removeGroup(const QString& group);
    void mergeGroups(const QString& unitedGroup, const QString& mergedGroup);
    void splitGroup(const QString& existGroup, const QString& newGroup, const QList<int>& movedIndicesInGroup);
    // Whole addressbook operation methods
    void hardSort(ContactList::SortType sortType);
    void massTagRemove(const QStringList& tagNames);
    void testList(); // Test data
    // Info
    void setViewMode(ContactViewMode mode, ContactModel* target);
    ContactViewMode viewMode();
    ContactList& itemList();
signals:
    void requestCSVProfile(CSVFile* format);
public slots:
protected:
#if QT_VERSION < 0x040600
    void beginResetModel() {};
    void endResetModel() { reset(); };
#endif
private:
    QString _source; // file path or network source
    FormatType _sourceType;
    bool _changed;   // has contact book unsaved changes?
    ContactList items;
    ContactColumnList visibleColumns;
    FormatFactory factory;
    ContactViewMode _viewMode;
    RecentList& _recent;
    bool checkForCSVProfile(IFormat* format, const QString& originalProfile);
};

#endif // CONTACTMODEL_H
