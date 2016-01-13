/* Double Contact
 *
 * Module: Data and container structures
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "contactlist.h"

const struct Phone fullPhoneFlagSet = { "0",  /*.tType =*/ Phone::All };

// We can't use QMap, etc, because multiple flags may be for one phone
QStringList Phone::typeToStrList() const
{
    QStringList res;
    if (tType.testFlag(Home))
        res.push_back(QObject::tr("Home"));
    if (tType.testFlag(Msg))
        res.push_back(QObject::tr("Msg"));
    if (tType.testFlag(Work))
        res.push_back(QObject::tr("Work"));
    if (tType.testFlag(Pref))
        res.push_back(QObject::tr("Pref"));
    if (tType.testFlag(Voice)) // Synonym for OTHER for some real phones
        res.push_back(QObject::tr("Voice"));
    if (tType.testFlag(Fax))
        res.push_back(QObject::tr("Fax"));
    if (tType.testFlag(Cell))
        res.push_back(QObject::tr("Cell"));
    if (tType.testFlag(Video))
        res.push_back(QObject::tr("Video"));
    if (tType.testFlag(Pager))
        res.push_back(QObject::tr("Pager"));
    if (tType.testFlag(BBS))
        res.push_back(QObject::tr("BBS"));
    if (tType.testFlag(Modem))
        res.push_back(QObject::tr("Modem"));
    if (tType.testFlag(Car))
        res.push_back(QObject::tr("Car"));
    if (tType.testFlag(ISDN))
        res.push_back(QObject::tr("ISDN"));
    if (tType.testFlag(PCS))
        res.push_back(QObject::tr("PCS"));
    return res;
}

QString Phone::typeToString() const
{
    return typeToStrList().join("+");
}

bool Phone::typeFromString(const QString &s)
{
    QStringList tt = s.split("+");
    foreach (const QString& src, tt) {
        if (src==QObject::tr("Home"))
            tType |= Home;
        else if (src==QObject::tr("Msg"))
            tType |= Msg;
        else if (src==QObject::tr("Work"))
            tType |= Work;
        else if (src==QObject::tr("Pref"))
            tType |= Pref;
        else if (src==QObject::tr("Voice"))
            tType |= Voice;
        else if (src==QObject::tr("Fax"))
            tType |= Fax;
        else if (src==QObject::tr("Cell"))
            tType |= Cell;
        else if (src==QObject::tr("Video"))
            tType |= Video;
        else if (src==QObject::tr("Pager"))
            tType |= Pager;
        else if (src==QObject::tr("BBS"))
            tType |= BBS;
        else if (src==QObject::tr("Modem"))
            tType |= Modem;
        else if (src==QObject::tr("Car"))
            tType |= Car;
        else if (src==QObject::tr("ISDN"))
            tType |= ISDN;
        else if (src==QObject::tr("PCS"))
            tType |= PCS;
        else return false;
    }
    return true;
}

void Phone::calculateFields()
{
    short flagCount = 0;
    if (tType.testFlag(Home))
        flagCount++;
    if (tType.testFlag(Msg))
        flagCount++;
    if (tType.testFlag(Work))
        flagCount++;
    if (tType.testFlag(Pref))
        flagCount++;
    if (tType.testFlag(Voice)) // Synonym for OTHER for some real phones
        flagCount++;
    if (tType.testFlag(Fax))
        flagCount++;
    if (tType.testFlag(Cell))
        flagCount++;
    if (tType.testFlag(Video))
        flagCount++;
    if (tType.testFlag(Pager))
        flagCount++;
    if (tType.testFlag(BBS))
        flagCount++;
    if (tType.testFlag(Modem))
        flagCount++;
    if (tType.testFlag(Car))
        flagCount++;
    if (tType.testFlag(ISDN))
        flagCount++;
    if (tType.testFlag(PCS))
        flagCount++;
    isMixed = (flagCount>1);
}

bool ContactItem::swapNames()
{
    if (names.isEmpty())
        return false;
    if (names.count()==1) names.push_back("");
    QString buffer = names[0];
    names[0] = names[1];
    names[1] = buffer;
    return true;
}

void ContactItem::calculateFields()
{
    prefPhone.clear(); // first or preferred phone number
    if (phones.count()>0) {
        prefPhone = phones[0].number;
        for (int i=0; i<phones.count();i++) {
            if (phones[i].tType.testFlag(Phone::Pref))
                prefPhone = phones[i].number;
            phones[i].calculateFields();
        }
    }
    prefEmail.clear(); // first or preferred email
    if (emails.count()>0) {
        prefEmail = emails[0].address;
        for (int i=0; i<emails.count(); i++)
            if (emails[i].preferred)
                prefEmail = emails[i].address;
    }
}

ContactList::ContactList()
{
}
