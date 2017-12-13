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
#ifndef ASYNCFORMAT_H
#define ASYNCFORMAT_H

#include <QObject>
#include "formats/iformat.h"

// Network errors
#define S_HOST_NOT_FOUND QObject::tr("Server %1 not found")
#define S_SRV_ERROR QObject::tr("Network error. Server %1 replied: %2")
#define S_CHECK_CONN QObject::tr("\nCheck username, password, protocol and port")

class IAsyncUI;

class AsyncFormat : public QObject, public IFormat
{
    Q_OBJECT
public:
    AsyncFormat();
    void setUI(IAsyncUI* ptr);
    QStringList errors();
    QString fatalError();
protected:
    enum State {
        StateOff,
        StateSSLRequest,
        StateConnect,
        StateTransfer,
        StateError
    } state;
    QString _url;
    QStringList _errors;
    QString _fatalError;
    IAsyncUI* ui;
    void pause(int msecs);
signals:
    void connected();
    void progress(const QString& stage, int progress, int total);
};

class IAsyncUI {
    virtual QString inputPassword() = 0;
    virtual bool securityConfirm(const QString& question) = 0;
};

#endif // ASYNCFORMAT_H
