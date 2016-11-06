/* Double Contact
 *
 * Module: console convertor application class
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "QFile"
#include "QFileInfo"
#include <QStringList>
#include "convertor.h"
#include "formats/formatfactory.h"
#include "formats/files/vcfdirectory.h"

Convertor::Convertor(int &argc, char **argv)
    : QCoreApplication(argc, argv),
      out(stdout)
{
}

// Main program loop
int Convertor::start()
{
    out << tr("Contact convertor by Mikhail Y. Zvyozdochkin\n");
    // Parse command line
    if (arguments().count()<2) {
        printUsage();
        return 1;
    }
    QString inPath, outPath, outFormat;
    bool forceOverwrite = false;
    bool forceSingleFile = false;
    bool forceDirectory = false;
    for (int i=1; i<arguments().count(); i++) {
        if (arguments()[i]=="-i") {
            i++;
            if (i==arguments().count()) {
                out << tr("linvert error: -i option present, but file path is missing\n");
                printUsage();
                return 2;
            }
            inPath = arguments()[i];
            continue;
        }
        else if (arguments()[i]=="-o") {
            i++;
            if (i==arguments().count()) {
                out << tr("linvert error: -o option present, but file path is missing\n");
                printUsage();
                return 3;
            }
            outPath = arguments()[i];
            continue;
        }
        else if (arguments()[i]=="-f") {
            i++;
            if (i==arguments().count()) {
                out << tr("linvert error: -f option present, but format name is missing\n");
                printUsage();
                return 4;
            }
            outFormat = arguments()[i];
            if (outFormat!="vcf21" && outFormat!="vcf30" && outFormat!="udx" && outFormat!="copy") {
                out << tr("Unknown output format: %1\n").arg(outFormat);
                printUsage();
                return 5;
            }
            continue;
        }
        else if (arguments()[i]=="-w")
            forceOverwrite = true;
        else if (arguments()[i]=="-s")
            forceSingleFile = true;
        else if (arguments()[i]=="-d")
            forceDirectory = true;
        else {
            out << tr("Unknown option: %1\n").arg(arguments()[i]);
            printUsage();
            return 6;
        }
    }
    // Check input data completion
    if (inPath.isEmpty()) {
        out << tr("Input path is missing\n");
        printUsage();
        return 7;
    }
    if (outPath.isEmpty()) {
        out << tr("Output path is missing\n");
        printUsage();
        return 8;
    }
    if (outFormat.isEmpty()) {
        out << tr("Output format name is missing\n");
        printUsage();
        return 9;
    }
    if (forceSingleFile && forceDirectory) {
        out << tr("Options -s and -d are not compatible\n");
        printUsage();
        return 10;
    }
    // Check if output file exists
    QFile of(outPath);
    if (of.exists() && !forceOverwrite && !QFileInfo(outPath).isDir()) {
        out << tr("Output file already exists, use -w if necessary\n");
        printUsage();
        return 11;
    }
    // Define, create file or directory at output
    // (default: as input)
    FormatType ift = QFileInfo(inPath).isDir() ? ftDirectory : ftFile;
    FormatType oft = ift;
    if (forceSingleFile)
        oft = ftFile;
    else if (forceDirectory)
        oft = ftDirectory;
    // Read
    IFormat* iFormat = createFormat(inPath, ift);
    if (!iFormat)
        return 12;
    ContactList items;
    bool res = iFormat->importRecords(inPath, items, false);
    logFormat(iFormat);
    delete iFormat;
    if (!res)
        return 13;
    out << tr("%1 records read\n").arg(items.count());
    // Write
    IFormat* oFormat = createFormat(outPath, oft);
    if (!oFormat)
        return 14;
    // TODO set format
    res = oFormat->exportRecords(outPath, items);
    logFormat(oFormat);
    delete oFormat;
    out << "Output file successfully written\n";
    return res ? 0 : 15;
}

// Print program usage if error occured
void Convertor::printUsage()
{
    out << tr(
        "Usage:\n" \
        "contconv -i inputfile -o outfile - f outformat [-w] [-d|-s]\n" \
        "\n" \
        "Possible values for outformat:\n" \
        "copy - same as input format, if atodetected (not works with MPB)\n" \
        "vcf21 - vCard version 2.1\n" \
        "vcf30 - vCard version 3.0\n" \
        "udx - Philips Xenium UDX\n" \
        "\n" \
        "Options:\n" \
        "-w - force overwrite output single file, if exists (directories overwrites already)\n" \
        "-s - write VCF as single file (by default, write as in input)\n" \
        "-d - write VCFs as directory (not compatible with -d)\n" \
        "\n"); // TODO filter
}

IFormat *Convertor::createFormat(const QString &path, FormatType ft)
{
    IFormat* format = 0;
    FormatFactory factory;
    if (ft==ftFile)
        format = factory.createObject(path);
    else
        format = new VCFDirectory();
    if (!format)
        out << factory.error << "\n";
    return format;
}

void Convertor::logFormat(IFormat* format)
{
    foreach (const QString& s, format->errors())
        out << s << "\n";
    if (!format->fatalError().isEmpty())
        out << "Error: " << format->fatalError() << "\n";
}
