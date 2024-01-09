/* Double Contact
 *
 * Module: Report helper for quick view, HTML report and office formats reports
 *
 * Copyright 2023 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "globals.h"
#include "textreport.h"

//TextReport::TextReport() {}

void TextReport::exportRecord(const ContactItem& item, RepItems& out, const QString& itemSeparator)
{
    // Name
    out.title = item.formatNames();
    // Phone(s), email(s)
    exportTypedItems(out, item.phones, S_PHONE,  itemSeparator);
    exportTypedItems(out, item.emails, S_EMAIL,  itemSeparator);
    // Birthday, anniversary
    exportStringableItem(out, item.birthday, S_BDAY);
    exportStringableItem(out, item.anniversary, S_ANN);
    exportString(out, item.description, S_DESC);
    // Group(s)
    if (!item.groups.isEmpty())
        exportString(out, item.groups.join(", "), S_GROUP); // space, not empty string!
    // Work
    exportString(out, item.organization, S_ORG);
    exportString(out, item.title, S_TITLE);
    exportString(out, item.title, S_ROLE);
    // Addresses
    exportTypedItems(out, item.addrs, S_ADDR,  itemSeparator);
    // Internet
    exportString(out, item.nickName, S_NICK);
    exportString(out, item.url, S_URL);
    exportTypedItems(out, item.ims, S_IM,  itemSeparator);

   // TODO See all ContactItem fields, including unknown and unsupported
    // Media if available
    if (!item.photo.isEmpty() && item.photo.pType=="URL")
        exportString(out, item.photo.url, S_PHOTO);
        // if JPEG or PNG, caller must process it!
}

void TextReport::exportString(RepItems &out, const QString &field, const QString &title)
{
    if (!field.isEmpty())
        out << QPair<QString, QString>(title, field); // old gcc 4.4.0 require <...>
}

template<class T>
void TextReport::exportStringableItem(RepItems &out, const T &field, const QString &title)
{
    if (!field.isEmpty())
        exportString(out, field.toString(), title);
}

template<class T>
void TextReport::exportTypedItems(RepItems &out, const QList<T> &lst, const QString &title, const QString& itemSeparator)
{
    if (!lst.isEmpty()) {
        int i=0;
        QString content;
        foreach (const T& it, lst) {
            QString types = "";
            for (int j=0; j<it.types.count(); j++) {
                types += it.standardTypes.translate(it.types[j]).toLower();
                if (j<it.types.count()-1)
                    types += "+";
            }
            content += QString("%1 (%2)").arg(it.toString(true)).arg(types);
            i++;
            if (i<lst.count())
                content += itemSeparator;
        }
        out << QPair<QString, QString>(title, content);
    }
}
