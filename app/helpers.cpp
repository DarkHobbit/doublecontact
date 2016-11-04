/* Double Contact
 *
 * Module: Widget helpers
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
#include "globals.h"
#include "helpers.h"

ReadOnlyTableDelegate::ReadOnlyTableDelegate(QObject* parent)
    : QItemDelegate(parent)
{}

QWidget* ReadOnlyTableDelegate::createEditor(
     QWidget*, const QStyleOptionViewItem&, const QModelIndex &) const
{
    QMessageBox::critical(0, S_ERROR, tr("This table is read only"));
    return 0;
}

