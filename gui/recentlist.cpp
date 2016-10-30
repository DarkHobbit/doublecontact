/* Double Contact
 *
 * Module: Recent opened file/directory list
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QSettings>
#include "recentlist.h"

RecentList::RecentList() : QStringList()
{}

void RecentList::read()
{
    clear();
    QSettings settings;
    int _count = settings.value("Recent/Count", 0).toInt();
    for (int i=1; i<=_count; i++)
        push_back(settings.value(QString("Recent/Item%1").arg(i)).toString());
}

void RecentList::write()
{
    QSettings settings;
    settings.setValue("Recent/Count", count());
    for (int i=1; i<=count(); i++)
        settings.setValue(QString("Recent/Item%1").arg(i), at(i-1));
    for (int i=count()+1; i<=MAX_RECENT_COUNT; i++)
        settings.remove(QString("Recent/Item%1").arg(i));
}

void RecentList::addItem(const QString &path)
{
    if (indexOf(path)!=-1)
        return;
    insert(0, path);
    if (count()>MAX_RECENT_COUNT)
        removeLast();
    write();
}

void RecentList::removeItem(const QString &path)
{
    removeOne(path);
    write();
}
