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

#ifndef RECENTLIST_H
#define RECENTLIST_H

#define MAX_RECENT_COUNT 10

#include <QStringList>

class RecentList : public QStringList
{
public:
    explicit RecentList();
    void read();
    void write();
    void addItem(const QString& path);
    void removeItem(const QString& path);
};

#endif // RECENTLIST_H
