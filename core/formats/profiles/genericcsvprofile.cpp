/* Double Contact
 *
 * Module: CSV file profile for full vCard data saving
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#include "genericcsvprofile.h"

int TypeCounter::totalCount()
{
    int res = 0;
    foreach(int nextCount, (*this))
        res += nextCount;
    return res;
}

void TypeCounter::append(const TypeCounter &localCounter)
{
    foreach (const QString& sType, localCounter.keys())
        if (localCounter[sType]>(*this)[sType])
                (*this)[sType] = localCounter[sType];
}

GenericCSVProfile::GenericCSVProfile()
{
    clearCounters();
    _name = QObject::tr("Generic profile");
    _hasHeader = true;
    _charSet = "UTF-8"; // TODO select encoding and separator in settings
    // TODO see, how ANISIMOV changes separator
    // To open this ANISIMOV CSV with ; separator in LibreOffice 3.6.1.2, set ";" separator in LibreOffice import window; else N components will splitted
    _hasBOM = false;
    _quotingPolicy = CSVProfileBase::AlwaysQuote;
    _lineEnding = CSVProfileBase::CRLFEnding;
}

bool GenericCSVProfile::detect(const QStringList &header) const
{
    return header.contains("N") || header.contains("FN");
}

bool GenericCSVProfile::parseHeader(const QStringList &header)
{
    _header = header;
    return (!header.isEmpty());
}

bool GenericCSVProfile::importRecord(const QStringList &row, ContactItem &item, QStringList& errors)
{
    // TODO - check if row length != header length
    return true;
}

bool GenericCSVProfile::prepareExport(const ContactList &list)
{
    if (list.isEmpty())
        return false;
    clearCounters();
    foreach (const ContactItem &c, list) {
        checkStr(c.version, hasVersion);
        checkStr(c.fullName, hasFullNames);
        if (!c.names.isEmpty())
            hasNames = true;
        checkTypeCombinations(c.phones, phoneTypeCombinations);
        checkTypeCombinations(c.emails, emailTypeCombinations);
        if (!c.birthday.isEmpty())
            hasBDay = true;
        if (!c.anniversaries.isEmpty())
            hasAnn = true;
        checkStr(c.sortString, hasSortString);
        checkStr(c.description, hasDesc);
        if (c.photo.pType=="URL")
            hasPhotoUrl = true;
        checkStr(c.organization, hasOrg);
        checkStr(c.title, hasTitle);
        // TODO role, logo?
        checkTypeCombinations(c.addrs, addrTypeCombinations);
        checkStr(c.nickName, hasNick);
        checkStr(c.url, hasUrl);
        checkTypeCombinations(c.ims, imTypeCombinations);
        // Unknown and other tags
        checkAnyTags(c.otherTags, otherTags);
        checkAnyTags(c.unknownTags, unknownTags);
    }
    return true;
}

QStringList GenericCSVProfile::makeHeader()
{
    QStringList header;
    makeHeaderItem(header, "VERSION", hasVersion);
    makeHeaderItem(header, "FN", hasFullNames);
    makeHeaderItem(header, "N", hasNames);
    makeHeaderGroup(header, "TEL", phoneTypeCombinations);
    makeHeaderGroup(header, "EMAIL", emailTypeCombinations);
    makeHeaderItem(header, "BDAY", hasBDay);
    makeHeaderItem(header, "X-ANNIVERSARY", hasAnn); // TODO anniversary if 40
    makeHeaderItem(header, "SORT-STRING", hasSortString);
    makeHeaderItem(header, "NOTE", hasDesc);
    makeHeaderItem(header, "PHOTO;VALUE=uri", hasPhotoUrl);
    makeHeaderItem(header, "ORG", hasOrg);
    makeHeaderItem(header, "TITLE", hasTitle);
    makeHeaderGroup(header, "ADR", addrTypeCombinations);
    makeHeaderItem(header, "NICKNAME", hasNick);
    makeHeaderItem(header, "URL", hasUrl);
    makeHeaderGroup(header, "IMPP", imTypeCombinations);
    // Unknown and other tags
    makeHeaderGroup(header, "", otherTags);
    makeHeaderGroup(header, "", unknownTags);
    return header;
}

bool GenericCSVProfile::exportRecord(QStringList &row, const ContactItem &item, QStringList& errors)
{
    putItem(row, item.version, hasVersion);
    putItem(row, item.fullName, hasFullNames);
    putItem(row, item.names.join(";"), hasNames);
    putTypeCombinations(row, item.phones, phoneTypeCombinations);
    putTypeCombinations(row, item.emails, emailTypeCombinations);
    putItem(row, item.birthday.toString(), hasBDay);
    if (hasAnn)
        row << item.anniversaries[0].toString();
    putItem(row, item.sortString, hasSortString);
    putItem(row, item.description, hasDesc);
    putItem(row, item.photo.url, hasPhotoUrl);
    if ((!item.photo.isEmpty()) && !hasPhotoUrl)
        errors << QString("%1 (%2) contains photo files, data will be lost")
                  .arg(item.fullName).arg(item.names.join(" "));
    // TODO for not-url phones, save in separate files
    putItem(row, item.organization, hasOrg);
    putItem(row, item.title, hasTitle);
    putTypeCombinations(row, item.addrs, addrTypeCombinations);
    putItem(row, item.nickName, hasNick);
    putItem(row, item.url, hasUrl);
    putTypeCombinations(row, item.ims, imTypeCombinations);
    // TODO Unknown and other tags
    putAnyTags(row, item.otherTags, otherTags);
    putAnyTags(row, item.unknownTags, unknownTags);
    return false;
}

void GenericCSVProfile::clearCounters()
{
    _header.clear();
    hasVersion = hasFullNames = hasNames = false;
    phoneTypeCombinations.clear();
    emailTypeCombinations.clear();
    addrTypeCombinations.clear();
    imTypeCombinations.clear();
    hasBDay = hasAnn = hasSortString = hasDesc = hasPhotoUrl = false;
    hasOrg = hasTitle = hasNick = hasUrl = false;
    otherTags.clear();
    unknownTags.clear();
}

void GenericCSVProfile::checkStr(const QString &value, bool &found)
{
    if (!value.isEmpty())
        found = true;
}

void GenericCSVProfile::checkAnyTags(const QList<TagValue> &tags, TypeCounter &combinations)
{
    TypeCounter localCounter;
    foreach(const TagValue& pair, tags) {
        if (localCounter.contains(pair.tag))
            localCounter[pair.tag]++;
        else
            localCounter[pair.tag] = 1;
    }
    // Compare localCounter and combinations
    combinations.append(localCounter);
}

template<class T>
void GenericCSVProfile::checkTypeCombinations(const QList<T> &values, TypeCounter &combinations)
{
    TypeCounter localCounter;
    // Collect types for each values
    // One type in one record can appear more than one time (two HOME phones)
    foreach(const T& t, values) {
        QStringList types;
        foreach (const QString& tt, t.types)
            types << tt.toUpper();
        types.sort();
        QString sTypes = types.join(";");
        if (localCounter.contains(sTypes))
            localCounter[sTypes]++;
        else
            localCounter[sTypes] = 1;
    }
    // Compare localCounter and combinations
    combinations.append(localCounter);
}

void GenericCSVProfile::makeHeaderGroup(QStringList& header, const QString& tagStart, TypeCounter &combinations)
{
    foreach(const QString& key, combinations.keys())
        for(int i=0; i<combinations[key]; i++)
            if (tagStart.isEmpty())
                header << key;
            else
                header << tagStart+";"+key;
}

void GenericCSVProfile::putItem(QStringList &row, const QString& value, bool condition)
{
    if (condition)
        row << value;
}

void GenericCSVProfile::putAnyTags(QStringList &row, const QList<TagValue> &data, const TypeCounter &combinations)
{
    foreach(const QString& tag, combinations.keys()) {
        int tagCount = combinations[tag];
        int dataIndex = 0;
        int written = 0;
        for (int i=0; i<tagCount; i++) {
            while (dataIndex<data.count()) {
                if (data[dataIndex].tag==tag) {
                    row << data[dataIndex].value;
                    written++;
                    dataIndex++;
                    break; // only from inner
                }
                dataIndex++;
            }
        }
        // If entries of this combination on contact less than maximum on addressbook...
        for (int i=written; i<tagCount; i++)
            row << "";
    }
}

template<class T>
void GenericCSVProfile::putTypeCombinations(QStringList &row, const QList<T> &data, const TypeCounter &combinations)
{
    foreach(const QString& key, combinations.keys()) {
        int combCount = combinations[key];
        int dataIndex = 0;
        int written = 0;
        for (int i=0; i<combCount; i++) {
            while (dataIndex<data.count()) {
                if (data[dataIndex].types.join(";").toUpper()==key) {
                    row << data[dataIndex].toString(false);
                    written++;
                    dataIndex++;
                    break; // only from inner
                }
                dataIndex++;
            }
        }
        // If entries of this combination on contact less than maximum on addressbook...
        for (int i=written; i<combCount; i++)
            row << "";
    }
}

