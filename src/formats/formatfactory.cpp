#include "formatfactory.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>

#include "files/vcffile.h"

FormatFactory::FormatFactory()
{
}

QStringList FormatFactory::supportedExtensions()
{
    QStringList allTypes;
    // Known formats
    allTypes << VCFFile::supportedTypes();
    // ...here add extensions for new format
    return allTypes;
}

IFormat *FormatFactory::createObject(const QString &url)
{
    if (url.isEmpty()) {
        QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Empty file name"));
        return 0;
    }
    QFileInfo info(url);
    QString ext = info.completeSuffix();
    // Known formats by extension
    if (VCFFile::supportedTypes().contains(ext, Qt::CaseInsensitive))
        return new VCFFile();
    // ...here add extensions for new format
    // Known formats with incorrect extension
    if (VCFFile::detect(url))
        return new VCFFile();
    // ...here add detect() for new format
    // Sad but true
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Unknown file format"));
    return 0;
}
