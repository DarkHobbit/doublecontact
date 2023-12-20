/* Double Contact
 *
 * Module: HTML file report
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "corehelpers.h"
#include "htmlfile.h"
#include "../common/textreport.h"

HTMLFile::HTMLFile()
{
}

QStringList HTMLFile::supportedExtensions()
{
    return (QStringList() << "html" << "HTML" << "htm" << "HTM");
}

QStringList HTMLFile::supportedFilters()
{
    return (QStringList() << "HTML (*.html *.HTML *.htm *.HTM)");
}


bool HTMLFile::importRecords(const QString &, ContactList &, bool)
{
    return false;
}

bool HTMLFile::exportRecords(const QString &url, ContactList &list)
{
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    _errors.clear();
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << QString("<html><head>\n<meta charset=\"utf-8\">\n<title>%1</title>\n</head>\n<body>").arg(url) << ENDL;
    // General data
    stream << QString("<b>%1</b>: %2<br/>\n").arg(S_ADDRESS_BOOK).arg(url);
    if (!list.extra.model.isEmpty())
        stream << QString("%1<br/>\n").arg(list.extra.model);
    if (list.extra.timeStamp.isValid())
        stream << QString("%1<br/>\n").arg(list.extra.timeStamp.toString());
    stream << ENDL;
    foreach (const ContactItem& item, list) {
        stream << QString("<p>\n");
        TextReport::RepItems out;
        TextReport::exportRecord(item, out, "; ");
        // Name
        stream << QString("<b>%1</b>\n").arg(out.title);
        // Content
        foreach (const TextReport::RepItem& sect, out)
            stream << QString("<br/><b>%1:</b> %2\n")
                .arg(sect.first).arg(sect.second) << ENDL;
        // Media if available
        if (!item.photo.isEmpty() && item.photo.pType!="URL") {
            stream << "<br/><b>" << S_HAS_PHOTO << "</b> ("
                   <<  item.photo.pType << ")" << ENDL; //===>
            // TODO here upload photo, if present && (PNG || JPEG), maybe through QByteArray from exportRecord->out QByteArray
        }
        stream << QString("</p>\n\n");
    }
    // Summary
    stream << QString("<hr/>\n\n");
    stream << list.statistics().replace("\n", "<br/>\n") << "\n\n";
    stream << "\n<body>\n<html>" << ENDL;
    closeFile();
    return true;
}
