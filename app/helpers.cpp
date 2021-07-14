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

#include <QFont>
#include <QHeaderView>
#include <QMessageBox>
#include <QPixmap>
#include <QSortFilterProxyModel>

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

// Show photo on label
void showPhoto(const Photo &photo, QLabel *label)
{
    label->clear();
    QString pt = photo.pType.toUpper();
    if (pt=="URL")
        label->setText(photo.url);
    else if (pt=="JPEG" || pt=="PNG") {
        QPixmap pixPhoto;
        pixPhoto.loadFromData(photo.data);
        // QLabel::scaledContents works ugly. More nice decision - scale only big images
        // and keep photo aspect ratio.
        if ((pixPhoto.width()>label->width()) || (pixPhoto.height()>label->height()))
            pixPhoto = pixPhoto.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(pixPhoto);
    }
    else if (!photo.isEmpty())
        label->setText(S_PH_UNKNOWN_FORMAT);
}

// Set color/font for each table view
void updateTableConfig(QTableView *table)
{
    table->setShowGrid(gd.showTableGrid);
    table->verticalHeader()->setVisible(gd.showLineNumbers);
    table->setAlternatingRowColors(gd.useTableAlternateColors);
    if (!gd.useSystemFontsAndColors) {
        QFont f;
        bool fontSuccess = f.fromString(gd.tableFont);
        if (fontSuccess)
            table->setFont(f);
        table->setStyleSheet(QString("QTableView { alternate-background-color: %1; background: %2 }")
               .arg(gd.gridColor2).arg(gd.gridColor1));
    }
}

void readTableSortConfig(QTableView *table, bool forceNeedSort, bool needSort)
{
    bool enabled;
    int column;
    Qt::SortOrder order;
    configManager.readSortConfig(table->objectName(), enabled, column, order);
    if (forceNeedSort)
        enabled = needSort;
    if (!enabled)
        column = -1;
    table->setSortingEnabled(enabled);
    if (enabled)
        table->horizontalHeader()->setSortIndicator(column, order);
}

void writeTableSortConfig(QTableView *table)
{
    QHeaderView *header = table->horizontalHeader();
    configManager.writeSortConfig(table->objectName(),
           table->isSortingEnabled(),
           header->sortIndicatorSection(),
           header->sortIndicatorOrder());
}

void writeTableSortConfig(QHeaderView* header)
{
    QTableView* table = dynamic_cast<QTableView*>(header->parent());
    if (table)
        configManager.writeSortConfig(table->objectName(),
               table->isSortingEnabled(),
               header->sortIndicatorSection(),
               header->sortIndicatorOrder());
}
