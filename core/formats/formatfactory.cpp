#include "formatfactory.h"
#include <QFileInfo>
#include <QObject>

#include "files/mpbfile.h"
#include "files/udxfile.h"
#include "files/vcffile.h"
#include "files/csvfile.h"

FormatFactory::FormatFactory()
    :error("")
{
}

QStringList FormatFactory::supportedFilters(QIODevice::OpenMode mode)
{
    QStringList allTypes;
    // Known formats (all supported)
    QString allSupported;
    allSupported += "*." + VCFFile::supportedExtensions().join(" *.");
    allSupported += "*." + UDXFile::supportedExtensions().join(" *.");
#if QT_VERSION >= 0x040800
    allSupported += "*." + MPBFile::supportedExtensions().join(" *.");
#else
#warning MPB save and load will not work under Qt earlier than 4.8
#endif
    if (mode==QIODevice::ReadOnly) {
        // ...here add read-only formats
        //allSupported += "*." + CSVFile::supportedExtensions().join(" *.");
    }
    // ...here add supportedExtensions() for new format
    allTypes << S_ALL_SUPPORTED.arg(allSupported);
    // Known formats (separate)
    allTypes << VCFFile::supportedFilters();
    allTypes << UDXFile::supportedFilters();
#if QT_VERSION >= 0x040800
    allTypes << MPBFile::supportedFilters();
#endif
    if (mode==QIODevice::ReadOnly) {
        // ...here add filters for read-only formats
        //allTypes << CSVFile::supportedFilters();
    }
    // ...here add supportedFilters() for new format
    allTypes << S_ALL_FILES;
    return allTypes;
}

IFormat *FormatFactory::createObject(const QString &url)
{
    if (url.isEmpty()) {
        error = QObject::tr("Empty file name");
        return 0;
    }
    QFileInfo info(url);
    QString ext = info.completeSuffix();
    // Known formats by extension
    if (VCFFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new VCFFile();
    if (UDXFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new UDXFile();
    if (CSVFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new CSVFile();
#if QT_VERSION >= 0x040800
    if (MPBFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new MPBFile();
#endif
    // ...here add supportedExtensions() for new format
    // Known formats with non-standard extension
    if (VCFFile::detect(url))
        return new VCFFile();
    if (UDXFile::detect(url))
        return new UDXFile();
    if (CSVFile::detect(url))
        return new CSVFile();
    // ...here add detect() for new format
    // Sad but true
    error = QObject::tr("Unknown file format:\n%1").arg(url);
    return 0;
}
