/* Double Contact
 *
 * Module: Creator of file/network export/import format classes
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef FORMATFACTORY_H
#define FORMATFACTORY_H

#include <QStringList>
#include "iformat.h"

class FormatFactory
{
public:
    FormatFactory();
    static QStringList supportedFilters();
    IFormat* createObject(const QString& url);
};

#endif // FORMATFACTORY_H
