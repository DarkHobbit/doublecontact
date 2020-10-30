/* Double Contact
 *
 * Module: console convertor main module
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "convertor.h"
#ifdef Q_WS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_WS_WIN
    Sleep(500); // for Qt Creator for Windows
#endif
    Convertor a(argc, argv);
    int res = a.start();
    a.exit(res);
    return res;
}
