/* Double Contact
 *
 * Module: Abstract class for network/device io with connection and timeouts
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QMutex>
#include "asyncformat.h"

AsyncFormat::AsyncFormat()
    :state(StateOff), ui(0)
{
}

void AsyncFormat::setUI(IAsyncUI *ptr)
{
    ui = ptr;
}

QStringList AsyncFormat::errors()
{
    return _errors;
}

QString AsyncFormat::fatalError()
{
    return _fatalError;
}

void AsyncFormat::pause(int msecs)
{
    QMutex mutex;
    mutex.lock();
    mutex.tryLock(msecs);
    mutex.unlock();
}
