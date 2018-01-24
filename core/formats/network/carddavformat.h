/* Double Contact
 *
 * Module: CardDAV protocol support
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QStringList>

#include "asyncformat.h"
#include "qwebdavdirparser.h"
#include "qwebdav.h"
#include "../common/vcarddata.h"

#ifndef CARDDAVFORMAT_H
#define CARDDAVFORMAT_H

# define PATH_CARDDAV_OWNCLOUD QString("/remote.php/carddav/addressbooks/%1/contacts")

class CardDAVFormat : public AsyncFormat, VCardData
{
    Q_OBJECT
public:
    explicit CardDAVFormat();
    virtual bool importRecords(const QString& url, ContactList& list, bool append);
    virtual bool exportRecords(const QString& url, ContactList& list);
private:
    QWebdav w;
    QWebdavDirParser p;
    QString host; // for valid error messages
    // Certificate details
    QStringList sslMessages;
    QString digMd5, digSha1;
    // Reading data
    ContactList* readingList;
public slots:
    void processSslCertifcate(const QList<QSslError> &errors);
    void onError(QString s);
    void onFinish();
    void replyFinished();
    void writeFinished();
    //void urlReqFinished();
    //void urlReqError(QNetworkReply::NetworkError code);
};

#endif // CARDDAVFORMAT_H
