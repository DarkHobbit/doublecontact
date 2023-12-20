/* Double Contact
 *
 * Module: Report helper for quick view, HTML report and office formats reports
 *
 * Copyright 2023 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef TEXTREPORT_H
#define TEXTREPORT_H

#include <QList>
#include <QPair>
#include <QString>
#include "../../contactlist.h"

class TextReport
{
public:
    //TextReport();
    typedef QPair<QString, QString> RepItem;
    struct RepItems: public QList<RepItem> {
        QString title;
    };
    static void exportRecord(const ContactItem& item, RepItems& out, const QString& itemSeparator);
    static void exportString(RepItems& items, const QString& field, const QString& title);
    template <class T>
    static  void exportStringableItem(RepItems& items, const T& field, const QString& title);
    template <class T>
    static  void exportTypedItems(RepItems& items, const QList<T>& lst,
       const QString& title, const QString& itemSeparator);
};

#endif // TEXTREPORT_H
