#include "formatfactory.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QObject>

#include "files/mpbfile.h"
#include "files/udxfile.h"
#include "files/vcffile.h"

FormatFactory::FormatFactory()
{
}

QStringList FormatFactory::supportedFilters(QIODevice::OpenMode mode)
{
    QStringList allTypes;
    // Known formats (all supported)
    QString allSupported;
    allSupported += "*." + VCFFile::supportedExtensions().join(" *.");
    allSupported += "*." + UDXFile::supportedExtensions().join(" *.");
    if (mode==QIODevice::ReadOnly) {
        allSupported += "*." + MPBFile::supportedExtensions().join(" *.");
    }
    // ...here add supportedExtensions() for new format
    allTypes << QObject::tr("All supported files (%1)").arg(allSupported);
    // Known formats (separate)
    allTypes << VCFFile::supportedFilters();
    allTypes << UDXFile::supportedFilters();
    if (mode==QIODevice::ReadOnly) {
        allTypes << MPBFile::supportedFilters();
    }
    // ...here add supportedFilters() for new format
    allTypes << QObject::tr("All files (*.*)");
    return allTypes;
}

IFormat *FormatFactory::createObject(const QString &url)
{
    if (url.isEmpty()) {
        QMessageBox::critical(0, S_ERROR, QObject::tr("Empty file name"));
        return 0;
    }
    QFileInfo info(url);
    QString ext = info.completeSuffix();
    // Known formats by extension
    if (VCFFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new VCFFile();
    if (UDXFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new UDXFile();
    if (MPBFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new MPBFile();
    // ...here add supportedExtensions() for new format
    // Known formats with non-standard extension
    if (VCFFile::detect(url))
        return new VCFFile();
    if (UDXFile::detect(url))
        return new UDXFile();
    // ...here add detect() for new format
    // Sad but true
    QMessageBox::critical(0,
        S_ERROR, QObject::tr("Unknown file format:\n%1").arg(url));
    return 0;
}
