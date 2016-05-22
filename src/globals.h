/* Double Contact
 *
 * Module: Global definitions
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QHash>
#include <QItemDelegate>
#include <QStringList>

// Visible columns
enum ContactColumn {
  ccFirstName, ccLastName, ccMiddleName, // names
  ccFullName, ccGenericName, // formatted names
  ccPhone, // first or preferred phone
  ccEMail, // first or preferred email
  ccLast
};

// Visible columns headers
extern const QString contactColumnHeaders[ccLast];

class ContactColumnList: public QList<ContactColumn>
{};

// Standart support types (phone, email, address) storage and checker
class StandardTypes: public QHash<QString, QString> {
    public:
    // untranslated keys stored in lowercase
    QString translate(const QString& key, bool* isStandard = 0) const;
    QString unTranslate(const QString& value) const;
    QStringList displayValues;
};

// Delegate for making table widgets read only
// Thanx to Axis - http://axis.bplaced.net/news/460
class ReadOnlyTableDelegate : public QItemDelegate
{
public:
    ReadOnlyTableDelegate(QObject* parent = 0);
    virtual QWidget* createEditor(
        QWidget*, const QStyleOptionViewItem&, const QModelIndex &) const;
};


#endif // GLOBALS_H
