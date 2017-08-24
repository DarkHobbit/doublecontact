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
#include <QPixmap>

#include "globals.h"
#include "helpers.h"

ReadOnlyTableDelegate::ReadOnlyTableDelegate(bool _silent, QObject* parent)
    : QItemDelegate(parent), silent(_silent)
{}

QWidget* ReadOnlyTableDelegate::createEditor(
     QWidget*, const QStyleOptionViewItem&, const QModelIndex &) const
{
    if (!silent)
        QMessageBox::critical(0, S_ERROR, tr("This table is read only"));
    return 0;
}

void showPhoto(const Photo &photo, QLabel *label)
{
    label->clear();
    QString pt = photo.pType.toUpper();
    if (pt=="URL")
        label->setText(photo.url);
    else if (pt=="JPEG" || pt=="PNG") {
        QPixmap pixPhoto;
        pixPhoto.loadFromData(photo.data);
        label->setPixmap(pixPhoto);
    }
    else if (!photo.isEmpty())
        label->setText(S_PH_UNKNOWN_FORMAT);
}
