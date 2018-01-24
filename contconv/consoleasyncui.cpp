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
#include <QObject>
#include "consoleasyncui.h"

ConsoleAsyncUI::ConsoleAsyncUI()
  : out(stdout), in(stdin)
{
}

QString ConsoleAsyncUI::inputLogin()
{
    out << QObject::tr("Enter your user name:");
    out.flush();
    QString login = in.readLine();
    return login;
}

QString ConsoleAsyncUI::inputPassword()
{
    out << QObject::tr("Enter your password:");
    out.flush();
    echo(false);
    QString pass = in.readLine();
    echo(true);
    out << "\n";
    //out << "you entered: " << pass << "\n"; //===>
    return pass;
}

bool ConsoleAsyncUI::securityConfirm(const QString &question)
{
    out << question << " (y|n)";
    out.flush();
    int answer = getchar();
    return (answer=='y');
}

// http://www.cplusplus.com/forum/general/12256/
#ifdef WIN32
#include <windows.h>

void ConsoleAsyncUI::echo(bool on)
{
    DWORD  mode;
    HANDLE hConIn = GetStdHandle( STD_INPUT_HANDLE );
    GetConsoleMode( hConIn, &mode );
    mode = on
            ? (mode |   ENABLE_ECHO_INPUT )
            : (mode & ~(ENABLE_ECHO_INPUT));
    SetConsoleMode( hConIn, mode );
}

#else
#include <termios.h>
#include <unistd.h>

void ConsoleAsyncUI::echo(bool on)
{
    struct termios settings;
    tcgetattr( STDIN_FILENO, &settings );
    settings.c_lflag = on
            ? (settings.c_lflag |   ECHO )
            : (settings.c_lflag & ~(ECHO));
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
}

#endif
