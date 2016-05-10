#include "formatfactory.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>

#include "files/vcffile.h"
#include "files/udxfile.h"

FormatFactory::FormatFactory()
{
}

QStringList FormatFactory::supportedFilters()
{
    QStringList allTypes;
    // Known formats
    allTypes << VCFFile::supportedFilters();
    allTypes << UDXFile::supportedFilters();
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
    if (VCFFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new VCFFile();
    if (UDXFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new UDXFile();
    // ...here add extensions for new format
    // Known formats with non-standard extension
    if (VCFFile::detect(url))
        return new VCFFile();
    if (UDXFile::detect(url))
        return new UDXFile();
    // ...here add detect() for new format
    // Sad but true
    QMessageBox::critical(0,
        QObject::tr("Error"),
        QObject::tr("Unknown file format:\n%1").arg(url));
    return 0;
}
