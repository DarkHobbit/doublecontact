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
#include "decodedmessagelist.h"
#include "formats/formatfactory.h"
#include "formats/common/vmessagedata.h"
#include "formats/files/htmlfile.h"
#include "formats/files/mpbfile.h"
#include "formats/files/nbffile.h"
#include "formats/files/udxfile.h"
#include "formats/files/vcfdirectory.h"
#include "formats/files/vcffile.h"
#ifdef WITH_NETWORK
#include "formats/network/carddavformat.h"
#include "formats/network/imageloader.h"
#endif

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
    QString inPath, outPath, inFormat, outFormat, inProfile, outProfile, filterString;
    ContactList::SortType sortType = ContactList::SortBySortString;
    bool infoMode = false;
    bool forceOverwrite = false;
    bool forceSingleFile = false;
    bool forceDirectory = false;
    bool swapNames = false;
    bool splitNames = false;
    bool generateFullNames = false;
    bool dropFullNames = false;
    bool reverseFullNames = false;
    bool dropSlashes = false;
    bool hardSort = false;
    bool filterExclusive = false;
    bool filterReverse = false;
    for (int i=1; i<arguments().count(); i++) {
        if (arguments()[i]=="-i" || arguments()[i]=="--info") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: %1 option present, but file path is missing\n").arg(arguments()[i-1]);
                printUsage();
                return 2;
            }
            inPath = arguments()[i];
            if (arguments()[i-1]=="--info")
                    infoMode = true;
            continue;
        }
        else if (arguments()[i]=="-o") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: -o option present, but file path is missing\n");
                printUsage();
                return 3;
            }
            outPath = arguments()[i];
            continue;
        }
        else if (arguments()[i]=="-n") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: -n option present, but format name is missing\n");
                printUsage();
                return 4;
            }
            inFormat = arguments()[i];
            if (inFormat!="carddav") {
                out << tr("Error: Unknown input format: %1\n").arg(outFormat);
                printUsage();
                return 5;
            }
            continue;
        }
        else if (arguments()[i]=="-f") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: -f option present, but format name is missing\n");
                printUsage();
                return 6;
            }
            outFormat = arguments()[i];
            if (outFormat!="vcf21" && outFormat!="vcf30" && outFormat!="vcf40"
                    && outFormat!="vcfauto"
            && outFormat!="udx" && outFormat!="mpb" && outFormat!="csv"
                    && outFormat!="html" && outFormat!="sms"
            && outFormat!="copy") {
                out << tr("Error: Unknown output format: %1\n").arg(outFormat);
                printUsage();
                return 7;
            }
            continue;
        }
        else if (arguments()[i]=="-ip") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: -ip option present, but profile name is missing\n");
                printUsage();
                return 8;
            }
            inProfile = arguments()[i];
            if (inProfile!="explaybm50" && inProfile!="explaytv240" && inProfile!="generic") {
                out << tr("Error: Unknown input profile: %1\n").arg(inProfile);
                printUsage();
                return 9;
            }
            continue;
        }
        else if (arguments()[i]=="-op") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: -op option present, but profile name is missing\n");
                printUsage();
                return 10;
            }
            outProfile = arguments()[i];
            if (outProfile!="explaybm50" && outProfile!="explaytv240" && outProfile!="generic") {
                out << tr("Error: Unknown output profile: %1\n").arg(outProfile);
                printUsage();
                return 11;
            }
            continue;
        }
        else if (arguments()[i]=="-w")
            forceOverwrite = true;
        else if (arguments()[i]=="-s")
            forceSingleFile = true;
        else if (arguments()[i]=="-d")
            forceDirectory = true;
        else if (arguments()[i]=="-fe")
            filterExclusive = true;
        else if (arguments()[i]=="-fr")
            filterReverse = true;
        else if (arguments()[i]=="--swap-names")
            swapNames = true;
        else if (arguments()[i]=="--split-names")
            splitNames = true;
        else if (arguments()[i]=="--generate-full-names")
            generateFullNames = true;
        else if (arguments()[i]=="--drop-full-names")
            dropFullNames = true;
        else if (arguments()[i]=="--reverse-full-names")
            reverseFullNames = true;
        else if (arguments()[i]=="--drop-slashes")
            dropSlashes = true;
        else if (arguments()[i]=="--sort") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: --sort command present, but sort criterion is missing\n");
                printUsage();
                return 12;
            }
            hardSort = true;
            QString c = arguments()[i];
            if (c=="ss")
                sortType = ContactList::SortBySortString;
            else if (c=="ln")
                sortType = ContactList::SortByLastName;
            else if (c=="frn")
                sortType = ContactList::SortByFirstName;
            else if (c=="fln")
                sortType = ContactList::SortByFullName;
            else if (c=="nn")
                sortType = ContactList::SortByNick;
            else {
                out << tr("Error: Unknown sort criterion: %1\n").arg(outProfile);
                printUsage();
                return 13;
            }
        }
        else if (arguments()[i]=="--filter") {
            i++;
            if (i==arguments().count()) {
                out << tr("Error: --filter command present, but filter string is missing\n");
                printUsage();
                return 14;
            }
            filterString = arguments()[i];
        }
        else {
            out << tr("Unknown option: %1\n").arg(arguments()[i]);
            printUsage();
            return 15;
        }
    }
    // Check input data completion
    if (inPath.isEmpty()) {
        out << tr("Error: Input path is missing\n");
        printUsage();
        return 16;
    }
    if (outPath.isEmpty() && !infoMode) {
        out << tr("Error: Output path is missing\n");
        printUsage();
        return 17;
    }
    if (outFormat.isEmpty() && !infoMode) {
        out << tr("Error: Output format name is missing\n");
        printUsage();
        return 18;
    }
    if (outFormat=="csv") {
        if (outProfile.isEmpty()) {
            out << tr("Error: Output format is CSV, but profile name is missing\n");
            printUsage();
            return 19;
        }
    }
    else {
        if (!outProfile.isEmpty()) {
            out << tr("Error: Output format isn't' CSV, but profile name is present\n");
            printUsage();
            return 20;
        }
    }
    if (forceSingleFile && forceDirectory) {
        out << tr("Error: Options -s and -d are not compatible\n");
        printUsage();
        return 21;
    }
    if (forceDirectory && !outFormat.contains("vcf") && outFormat!="copy") {
        out << tr("Error: -d option applicable only for vCard format");
        return 22;
    }
    if (infoMode && !(outPath.isEmpty() && outFormat.isEmpty())) {
        out << tr("Error: Command --info is not compatible with -o and -f options\n");
        printUsage();
        return 23;
    }
    if ((filterExclusive || filterReverse) && filterString.isEmpty()) {
        out << tr("Error: -fe and -fr option applicable only with --filter command");
        return 24;
    }
    // Check if output file exists
    QFile of(outPath);
    if (of.exists() && !forceOverwrite && !QFileInfo(outPath).isDir()) {
        out << tr("Error: Output file already exists, use -w if necessary\n");
        printUsage();
        return 25;
    }
    // Define, create file or directory at output
    // (default: as input)
    FormatType ift;
    if (inFormat.toLower()=="carddav") {
#ifdef WITH_NETWORK
        ift = ftNetwork;
#else
        out << S_ERR_NETWORK_SUPPORT;
        return 25;
#endif
    }
    else
        ift = QFileInfo(inPath).isDir() ? ftDirectory : ftFile;
    FormatType oft = ift;
    if (forceSingleFile)
        oft = ftFile;
    else if (forceDirectory)
        oft = ftDirectory;
    // Read
    IFormat* iFormat = 0;
    FormatFactory factory;
    if (ift==ftFile)
        iFormat = factory.createObject(inPath);
    else if (ift==ftDirectory)
        iFormat = new VCFDirectory();
#ifdef WITH_NETWORK
    else { // Network
        out << tr("Trying connect to server/device...\n");
        iFormat = new CardDAVFormat(); // TODO to factory, if more formats
        AsyncFormat* asf = dynamic_cast<AsyncFormat*>(iFormat);
        if (asf)
            asf->setUI(&aui);
    }
#endif
    if (!iFormat) {
        out << factory.error << "\n";
        return 26;
    }
    // Input CSV profile
    CSVFile* csvFormat = dynamic_cast<CSVFile*>(iFormat);
    if (csvFormat) {
        if (inProfile.isEmpty()) {
            out << tr("Error: Input format is CSV, but profile name is missing\n");
            printUsage();
            delete iFormat;
            return 27;
        }
        else
            setCSVProfile(csvFormat, inProfile);
    }
    out.flush();
    ContactList items;
    bool res = iFormat->importRecords(inPath, items, false);
    logFormat(iFormat);
    delete iFormat;
    if (!res)
        return 28;
    out << tr("%1 records read\n").arg(items.count());
    // Show statistics, if info mode switched on
    if (infoMode) {
        out << "\n" << items.statistics() << "\n";
        return 0;
    }
    // Load images, if possible
    if (false) { // TODO add option
#ifdef WITH_NETWORK
        ImageLoader imgLdr;
#else
        out << S_ERR_NETWORK_SUPPORT;
        return 25;
#endif
    }
    // Conversions
    for (int i=0; i<items.count(); i++) {
        ContactItem& item = items[i];
        bool filtered = true;
        if (!filterString.isEmpty()) {
            filtered = item.fullName.contains(filterString);
            if (!filtered)
                foreach (const QString& name, item.names)
                    if (name.contains(filterString))
                        filtered = true;
            if (!filtered)
                if (item.description.contains(filterString))
                    filtered = true;
            if (!filtered)
                foreach (const Phone& p, item.phones)
                    if (p.value.contains(filterString))
                        filtered = true;
            if (!filtered)
                foreach (const Phone& p, item.phones)
                    if (p.value.contains(filterString))
                        filtered = true;
            if (!filtered)
                foreach (const Email& m, item.emails)
                    if (m.value.contains(filterString))
                        filtered = true;
        }
        if (filterReverse)
            filtered = !filtered;
        if (filtered) {
            if (swapNames)
                item.swapNames();
            if (splitNames)
                item.splitNames();
            // TODO splitNumbers now can't be implemented, because in GUI it works via ContactModel
            // Probably, move it in ContactList in future
            if (generateFullNames)
                item.fullName = items[i].formatNames();
            if (dropFullNames)
                item.fullName.clear();
            if (reverseFullNames)
                item.reverseFullName();
            if (dropSlashes)
                item.dropSlashes();
            // TODO intlPhonePrefix implement after CountryManager create
            // items[i].intlPhonePrefix(cRule);
        }
        else if (filterExclusive) {
            items.removeAt(i);
            i--;
        }
    }
    // Sort
    if (hardSort)
        items.sort(sortType);
    //Define output format
    gd.preferredVCFVersion = GlobalConfig::VCF21;
    IFormat* oFormat = 0;
    if (oft==ftDirectory)
        oFormat = new VCFDirectory();
    else if (outFormat.contains("vcf")) {
        if (outFormat=="vcf30")
            gd.preferredVCFVersion = GlobalConfig::VCF30;
        else if (outFormat=="vcf40")
            gd.preferredVCFVersion = GlobalConfig::VCF40;
        gd.useOriginalFileVersion = (outFormat=="vcfauto");
        oFormat = new VCFFile();
    }
    else if (outFormat.contains("udx"))
        oFormat = new UDXFile();
    else if (outFormat.contains("mpb"))
        oFormat = new MPBFile();
    else if (outFormat.contains("csv"))
        oFormat = new CSVFile();
    else if (outFormat.contains("html"))
        oFormat = new HTMLFile();
    else if (outFormat.contains("copy")) { // copy input format
        if (VCFFile::detect(inPath)) {
            gd.useOriginalFileVersion = true;
            oFormat = new VCFFile();
        }
        else if (UDXFile::detect(inPath))
            oFormat = new UDXFile();
        else if (MPBFile::detect(inPath))
            oFormat = new MPBFile();
        else if (CSVFile::detect(inPath))
            oFormat = new CSVFile();
        else {
            out << "Error: Can't autodetect input format\n";
            return 29;
        }
    }
    // Output CSV profile
    csvFormat = dynamic_cast<CSVFile*>(oFormat);
    if (csvFormat)
        setCSVProfile(csvFormat, outProfile);
    // Write
    if (outFormat!="sms") {
        res = oFormat->exportRecords(outPath, items);
        logFormat(oFormat);
        delete oFormat;
        out << tr("%1 records written\n").arg(items.count());
    }
    else {
        QStringList msgErrors;
        DecodedMessageList messages = DecodedMessageList::fromContactList(items, msgErrors);
        if (!messages.isEmpty()) {
            res = messages.toCSV(outPath);
            if (res)
                out << tr("%1 messages written\n").arg(messages.count());
            else
                out << S_WRITE_ERR.arg(outPath) + "\n";
        }
        foreach (const QString& s, msgErrors)
            out << s << "\n";
    }
    return res ? 0 : 30;
}

// Print program usage
void Convertor::printUsage()
{
    out << tr(
        "Usage:\n" \
        "contconv -i inputfile -o outfile -f outformat [-n informat] [-ip csvprofile] [-op csvprofile] [-w] [-d|-s] [commands]\n" \
        "contconv --info inputfile\n" \
        "\n" \
        "Possible values for outformat:\n" \
        "copy - same as input format, if atodetected\n" \
        "csv - comma-separated values (-op option required)\n" \
        "vcf21 - vCard version 2.1\n" \
        "vcf30 - vCard version 3.0\n" \
        "vcf40 - vCard version 4.0\n" \
        "vcfauto - vCard version as in input file\n" \
        "udx - Philips Xenium UDX\n" \
        "mpb - MyPhoneExplorer backup\n" \
        "html - HTML report (write only)\n" \
        "sms - messages in csv file (for MPB, NFB or NBU input files only)"
        "\n" \
        "Possible values for csvprofile:\n" \
        "explaybm50, explaytv240, generic, osmo\n" \
        "\n" \
        "Options:\n" \
        "-w - force overwrite output single file, if exists (directories overwrites already)\n" \
        "-s - write VCF as single file (by default, write as in input)\n" \
        "-d - write VCFs as directory (not compatible with -d)\n" \
        "Commands:\n" \
        "--swap-names - swap first and last name\n" \
        "--split-names - split name by spaces\n" \
        "--generate-full-names - generate full (formatted) name by names\n" \
        "--drop-full-names - clear full (formatted) name\n" \
        "--reverse-full-names - swap parts of full (formatted) name\n"
        "--drop-slashes - remove back slashes and other SIM-legacy from names\n" \
        "--info - show statistic info about inputfile (incompatible with -o and -f options)\n" \
        "--sort criterion - hard sorting entire addressbook by criterion (see below)\n" \
        "--filter string [-fo] [-fr] - commands process only for records, where string found.\n" \
        "Search work in names, formatted names, descriptions, phones, emails.\n" \
        "If -fe option found, other records not recorded in output file. By default,\n" \
        "it recorded, but not processed by other commands (exclude --sort).\n" \
        "If -fr option found, filter is reversed (only records without string are filtered).\n" \
        "Possible values for sort criterion:\n" \
        "ss - contact sort string \n" \
        "ln - last name \n" \
        "frn - first name \n" \
        "fln - full name \n" \
        "nn - nickname \n" \
        "\n" \
        "Examples:\n" \
        "  contconv -i backup.vcf -o filtered.vcf -f copy -w --filter John -fe\n" \
        "  contconv -i https://ivan@192.168.56.101/remote.php/carddav/addressbooks/ivan/contacts -n carddav -o local.vcf -f vcf40 -w\n" \
        "  contconv -i https://ivan:uh67Er1B@192.168.56.101/remote.php/carddav/addressbooks/ivan/contacts -n carddav -o local.vcf -f vcf40 -w\n" \
               "\n");
}

void Convertor::logFormat(IFormat* format)
{
    foreach (const QString& s, format->errors())
        out << s << "\n";
    if (!format->fatalError().isEmpty())
        out << "Error: " << format->fatalError() << "\n";
}

void Convertor::setCSVProfile(CSVFile *csvFormat, const QString &code)
{
    if (code=="explaybm50")
        csvFormat->setProfile("Explay BM50");
    else if (code=="explaytv240")
        csvFormat->setProfile("Explay TV240");
    else if (code=="osmo")
        csvFormat->setProfile("Osmo PIM");
    else
        csvFormat->setProfile("Generic profile");
}
