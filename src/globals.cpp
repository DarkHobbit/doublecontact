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

#include <QMessageBox>
#include <QObject>
#include "globals.h"

// Visible columns headers
const QString contactColumnHeaders[ccLast] = {
    QObject::tr("Firstname"),
    QObject::tr("Lastname"),
    QObject::tr("Middlename"),
    QObject::tr("Full name"),
    QObject::tr("Generic name"),
    QObject::tr("Phone"),
    QObject::tr("eMail")
};

QString StandardTypes::translate(const QString &key, bool* isStandard) const
{
    if (this->contains(key.toLower())) {
        if (isStandard) *isStandard = true;
        return (*this)[key.toLower()];
    }
    else { // Non-standard type stored as is
        if (isStandard) *isStandard = false;
        return key;
    }
}

QString StandardTypes::unTranslate(const QString &value) const
{
    return key(value, value);
}

ReadOnlyTableDelegate::ReadOnlyTableDelegate(QObject* parent)
    : QItemDelegate(parent)
{}

QWidget* ReadOnlyTableDelegate::createEditor(
     QWidget*, const QStyleOptionViewItem&, const QModelIndex &) const
{
    QMessageBox::critical(0, tr("Error"), tr("This table is read only"));
    return 0;
}

