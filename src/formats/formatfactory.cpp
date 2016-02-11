#include "formatfactory.h"
#include <QMessageBox>
#include <QObject>

FormatFactory::FormatFactory()
{
    // TODO
}

QStringList FormatFactory::supportedExtensions()
{
    // TODO
    return QStringList(); //==>
}

IFormat *FormatFactory::createObject(const QString &url)
{
    if (url.isEmpty()) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Empty file name"));
        return 0;
    }
    // TODO
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Unknown file format"));
    return 0;
}
