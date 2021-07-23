/* Double Contact
 *
 * Module: Model Helpers
 *
 * Copyright 2021 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QString>
#include "modelhelpers.h"

QVariant emptyItemsToEnd(const QVariant &src)
{
    if (src.toString().isEmpty())
        return QString("\uFFFD");
    else
        return src;
}
