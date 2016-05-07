/* Double Contact
 *
 * Module: Proxy model for contact sorting/filtering
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "contactsorterfilter.h"

ContactSorterFilter::ContactSorterFilter(QObject* parent):
    QSortFilterProxyModel(parent)
{
}

bool ContactSorterFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, Qt::DisplayRole);
    QVariant rightData = sourceModel()->data(right, Qt::DisplayRole);
    return leftData.toString()<rightData.toString();
}
