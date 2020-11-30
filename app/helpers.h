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

#ifndef HELPERS_H
#define HELPERS_H

#include <QItemDelegate>
#include <QLabel>
#include <QTableView>

#include "contactlist.h"

// Delegate for making table widgets read only
// Thanx to Axis - http://axis.bplaced.net/news/460
class ReadOnlyTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ReadOnlyTableDelegate(bool _silent, QObject* parent = 0);
    virtual QWidget* createEditor(
        QWidget*, const QStyleOptionViewItem&, const QModelIndex &) const;
private:
    bool silent;
};

// Show photo on label
void showPhoto(const Photo& photo, QLabel* label);

// Set color/font for each table view
void updateTableConfig(QTableView* table);


#endif // HELPERS_H
