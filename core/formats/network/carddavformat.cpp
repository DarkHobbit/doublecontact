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
#include <QCoreApplication>
#include <QUrl>
#include <QUuid>
#include "carddavformat.h"

CardDAVFormat::CardDAVFormat() :
    AsyncFormat(), readingList(0)
{
}

bool CardDAVFormat::importRecords(const QString &url, ContactList &list, bool append)
{
    // TODO Google stub - move to separete proc, 2-4 weeks
/*    w.setConnectionSettings(QWebdav::HTTPS,
        "https://www.googleapis.com", "/.well-known/carddav");
    w.propfind() */
    // TODO End of Google stub
    _url = url;
    QUrl u(_url);
    if (!u.isValid()) {
        _fatalError = tr("Invalid URL");
        return false;
    }
    int port = u.port();
    // Optional components
    if (port==-1) // Standard port, if absent
        port = (u.scheme()=="http" ? 80 : 443);
    QString password = u.password();
    if (password.isEmpty())
        password = ui->inputPassword();
    if (password.isEmpty())
        return false;
    // TODO username
    if (u.userName().isEmpty()) { //===>
        _fatalError = "User is Mandatory!!!";
        return false;
    }
    qDebug() << u.scheme() << " :// " << u.userName() << " : " << password << " @ " << u.host() << " : " << port << " / " << u.path(); //===>
    // TODO show connecting state
    // WebDAV settings
    if (!append)
        list.clear();
    readingList = &list;
    connect(&p, SIGNAL(finished()), this, SLOT(onFinish()));
    connect(&p, SIGNAL(errorChanged(QString)), this, SLOT(onError(QString)));
    connect(&w, SIGNAL(errorChanged(QString)), this, SLOT(onError(QString)));
    connect(&w, SIGNAL(checkSslCertifcate(QList<QSslError>)), this, SLOT(processSslCertifcate(QList<QSslError>)));
    digMd5 = "";
    digSha1 = "";
    do {
        state = StateConnect;
        w.setConnectionSettings(
            (u.scheme()=="http" ? QWebdav::HTTP : QWebdav::HTTPS),
            u.host(), u.path(), u.userName(), password, port, digMd5, digSha1);
        p.listDirectory(&w, "/"); // ==> only for read???
        do {
            qApp->processEvents();
            pause(50); // min 14 for Win8
        } while (state!=StateOff && state!=StateSSLRequest);
        if (state==StateSSLRequest) {
            qApp->processEvents();
            QString q = tr("There are security problems:\n    %1\nAre you want to accept this certificate anyway?")
                .arg(sslMessages.join("\n    "));
            if (!ui->securityConfirm(q)) {
                readingList = 0;
                return false;
            }
        }
    } while (state!=StateOff);
    // TODO writing test begin - still not work
/*    QUuid id = QUuid::createUuid();
    QString sId = id.toString();
    sId.remove("{");
    sId.remove("}");
    path = PATH_CARDDAV_OWNCLOUD.arg(user) + "/" + sId + ".vcf";
    QString cont = "BEGIN:VCARD\n";
    cont += "FN:Full newbie\n";
    cont += QString("UID:%1\n").arg(sId);
    cont += "END:VCARD\n";
    QNetworkReply* rc = w.mkdir(path);
    connect(rc, SIGNAL(finished()), this, SLOT(writeFinished()));
    while (!rc->isFinished())
        qApp->processEvents();
    qDebug() << path;
    QNetworkReply* rp = w.put(path, cont.toLocal8Bit().data());
    connect(rp, SIGNAL(finished()), this, SLOT(writeFinished()));
    while (!rp->isFinished())
        qApp->processEvents();*/
    // TODO writing test end
    readingList = 0;
    return _fatalError.isEmpty();
}

bool CardDAVFormat::exportRecords(const QString &url, ContactList &list)
{
    // TODO
    return false;
}

void CardDAVFormat::processSslCertifcate(const QList<QSslError> &errors)
{
    if (!errors.isEmpty()) {
        foreach(const QSslError& err, errors)
            sslMessages << err.errorString();
        QSslCertificate sslCert = errors[0].certificate();
        digMd5 = w.digestToHex(sslCert.digest(QCryptographicHash::Md5));
        digSha1 = w.digestToHex(sslCert.digest(QCryptographicHash::Sha1));
        state = StateSSLRequest;
    }
}

void CardDAVFormat::onError(QString s)
{
    if (state==StateSSLRequest)
        return;
    state = StateError;
    // Translate QNetworkReply messages

    if (s.startsWith("Host") && s.contains("not found"))
        _fatalError = S_HOST_NOT_FOUND.arg(host);
    else if (s.contains("server replied"))
        _fatalError = S_SRV_ERROR.arg(host).arg(s.mid(s.indexOf("replied: ")+QString("replied: ").length()))
                + S_CHECK_CONN;
    else if (s.contains("SSL handshake failed")) // google && owncloud, port 443
        _fatalError = tr("SSL handshake failed") + S_CHECK_CONN;
    // TODO bad request (yandex), "Method Not Allowed" (google). etc.
    // Connection timed out (google, 8443)
    else
        _fatalError = s;
}

void CardDAVFormat::onFinish()
{
    if (state==StateSSLRequest)
        return;
    else
    if (state!=StateError) {
        QList<QWebdavItem> list = p.getList();
        if (list.isEmpty())
            _fatalError = tr("No DAV items. It seems that this is not a CardDAV server.") + S_CHECK_CONN;
        else {
            QWebdavItem item;
            foreach(item, list)
                if (item.name().contains(".vcf")) {
                    // _errors << "Vcard item: " << item.name();
                    QNetworkReply *reply = w.get(item.path());
                    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
                    while (!reply->isFinished())
                        qApp->processEvents();
                }
                else
                _errors << tr("Strange vCard item: ") << item.name();
            // TODO
        }
    }
    state = StateOff;
}

void CardDAVFormat::replyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply) {
        QByteArray ba = reply->readAll();
        QStringList tags = QString::fromLocal8Bit(ba).split("\n");
        if (readingList)
            VCardData::importRecords(tags, *readingList, true, _errors);
    }
}

void CardDAVFormat::writeFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply) {
        QByteArray ba = reply->readAll();
        qDebug() << "Wr " << ba;
    }
}
