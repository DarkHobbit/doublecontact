/* Double Contact
 *
 * Module: Strange XML contact file export/import
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QDomNodeList>
#include <QStringList>
#include <QTextStream>
#include "xmlcontactfile.h"

XmlContactFile::XmlContactFile()
    :FileFormat(), QDomDocument()
{
}

bool XmlContactFile::detect(const QString &url)
{
    QFile file(url);
    // - file is readable
    if (!file.open( QIODevice::ReadOnly))
        return false;
    // - file is XML
    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();
    return !(doc.elementsByTagName("contact").isEmpty());
}

QStringList XmlContactFile::supportedExtensions()
{
    return (QStringList() << "xml" << "XML");
}

QStringList XmlContactFile::supportedFilters()
{
    return (QStringList() << "XML (*.xml *.XML)");
}

bool XmlContactFile::importRecords(const QString &url, ContactList &list, bool append)
{
    if (!openFile(url, QIODevice::ReadOnly))
        return false;
    // Read XML
    QString err_msg;
    int err_line, err_col;
    if (!setContent(&file, &err_msg, &err_line, &err_col)) {
        _errors << QObject::tr("Can't read content from file %1\n%2\nline %3, col %4\n")
            .arg(url).arg(err_msg).arg(err_line).arg(err_col);
        closeFile();
        return false;
    }
    closeFile();
    QDomNodeList nodes = elementsByTagName("contact");
    if (!append)
        list.clear();
    for(int i=0; i<nodes.count(); i++) {
        QStringList lines = nodes.at(i).toElement().text().split("\r\n");
        if (VCardData::importRecords(lines, list, true, _errors))
            list.last().originalFormat = "XML";
    }
    return true;
}

bool XmlContactFile::exportRecords(const QString &/*url*/, ContactList &/*list*/)
{
    /*
    QDomDocument::clear();
    QDomElement root = createElement("root");
    appendChild(root);
    foreach(const ContactItem& item, list) {
        QDomElement e = createElement("contact");
        QStringList lines;
        VCardData::exportRecord(lines, item, _errors);
        QDomText t = createTextNode(lines.join("\r\n"));
        e.appendChild(t);
        root.appendChild(e);
    }
    QString content = toString(0);
    // Write to file
    if (!openFile(url, QIODevice::WriteOnly))
        return false;
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << content;
    closeFile();
    return true;
    */
    return false;
}


