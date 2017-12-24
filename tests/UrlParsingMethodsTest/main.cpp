#include <QCoreApplication>
#include <QDebug>
#include <QRegExp>
#include <QUrl>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Case 1. Regular Expressions

    QString url = "https://user:bitnami@192.168.56.101/remote.php/carddav/addressbooks/user/contacts";
    //QString url = "https://user@192.168.56.101/remote.php/carddav/addressbooks/user/contacts";
    //QString url = "https://192.168.56.101/remote.php/carddav/addressbooks/user/contacts";
    //QString url = "https://www.googleapis.com:443/carddav/v1/principals/YOUR@gmail.com/lists/default";

    // Regexp given from rfc3986, appendix B
    QRegExp rUrl(
                "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");
    if (!rUrl.isValid())
        qDebug() << "Invalid URI pattern";
    else
        if (!rUrl.exactMatch(url))
            qDebug() << "Invalid URL";
        else {
            qDebug() << "URL components:";
            for (int i=1; i<9; i++)
                qDebug() << rUrl.cap(i);
            qDebug() << "\n";
            /*
            QString au = rUrl.cap(4);
            if (au.isEmpty())
                qDebug() << "Authority is absent";
            else {
                rUrl.setPattern("([^/?#]*)?(:([0-9]+))?");
                rUrl.setPattern("([^/?#]*)?(:([0-9]+))");
                if (!rUrl.isValid())
                    qDebug() << "Invalid authority pattern";
                else {
                    qDebug() << "Authority components:";
                    if (rUrl.indexIn(au)!=-1) {
                        qDebug() << "pos: " << rUrl.indexIn(au);
                        qDebug() << "host: " << rUrl.cap(1);
                        qDebug() << "port: " << rUrl.cap(3);
                    }
                }

            }*/
        }

    // Case 2. QUrl

    QUrl u("https://user:bitnami@192.168.56.101:4443/remote.php/carddav/addressbooks/user/contacts");
    //QUrl u("https://user:bitnami@192.168.56.101/remote.php/carddav/addressbooks/user/contacts");
    //QUrl u("https://user@192.168.56.101:4443/remote.php/carddav/addressbooks/user/contacts");
    //QUrl u("https://192.168.56.101/remote.php/carddav/addressbooks/user/contacts");
    //QUrl u("https://www.googleapis.com/carddav/v1/principals/YOUR@gmail.com/lists/default");
    qDebug() << "";
    qDebug() << "Scheme: " << u.scheme();
    qDebug() << "User: " << u.userName();
    qDebug() << "Password: " << u.password();
    qDebug() << "Host: " << u.host();
    qDebug() << "Port: " << u.port();
    qDebug() << "Path: " << u.path();



    return 0;
}
