/* Double Contact
 *
 * Module: Console UI for network/device io with connection and timeouts
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef CONSOLEASYNCUI_H
#define CONSOLEASYNCUI_H

#include <QTextStream>
#include "formats/network/asyncformat.h"

class ConsoleAsyncUI : public IAsyncUI
{
public:
    ConsoleAsyncUI();
    virtual QString inputLogin();
    virtual QString inputPassword();
    virtual bool securityConfirm(const QString& question);
private:
    QTextStream out, in;
    // http://www.cplusplus.com/forum/general/12256/
    void echo(bool on=true);
};

#endif // CONSOLEASYNCUI_H
