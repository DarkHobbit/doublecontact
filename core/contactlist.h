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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QByteArray>
#include <QDateTime>
#include <QMap>
#include <QStringList>

#include "globals.h"

#define MAX_COMPARE_PRIORITY_LEVEL 5
#define COUNTRY_RULES_COUNT 3
#define MAX_NAMES 5
// According vCard 4.0, contact can have only one anniversary
#define MAX_ANN 1

// Tag+Value pair for read-only and unknown tags
// We can't use QMap<QString,QString>, because
// some tags with one tagname can be found
struct TagValue { // for non-editing ang unknown tags
    QString tag, value;
    TagValue(const QString& _tag, const QString& _value);
};

class TagList: public QList<TagValue> {
public:
    QStringList findByName(const QString& tagName) const;
    int findOneByName(const QString& tagName) const;
};

// vCard item with one of more types (one phone, email, impp, address, etc.)
struct TypedDataItem {
    QStringList types;
    // Phone: some devices & addressbooks may allow create any tel type (not RFC, but...)
    // Email: according RFC 2426, may be non-standard
    int syncMLRef;
    virtual ~TypedDataItem();
    virtual QString toString(bool humanReadable) const=0;
    template<class T>
    static const T* findByType(const QList<T>& list, const QString& itemType);
};

// vCard item with one of more types and string content (one phone, email, impp, etc.)
struct TypedStringItem: public TypedDataItem {
    QString value;
    virtual QString toString(bool) const;
};

struct Phone: public TypedStringItem {
    Phone();
    Phone(const QString& _value, const QString& type1 = "", const QString& type2 = "");
    bool operator ==(const Phone& p) const;
    // international number representation from particular
    static QStringList availableCountryRules();
    QString expandNumber(int countryRule) const;
    static QString expandNumber(const QString& number, int countryRule);
    // standart types
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

struct Email: public TypedStringItem {
    Email();
    Email(const QString& _value, const QString& type1 = "", const QString& type2 = "");
    bool operator ==(const Email& e) const;
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

struct Messenger: public TypedStringItem {
    Messenger();
    Messenger(const QString& _value, const QString& type1 = "", const QString& type2 = "");
    bool operator ==(const Messenger& m) const;
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

// TODO Maybe, after total port to Qt 5.10+, drop this type
// and use QDateTime with timezoe support instead it
// OR make TZDateTime from this class and use it for older Qt
// Old Qt - inherit from QDateTime, new Qt - TZDateTime==QDateTime
struct DateItem { // Birthday and anniversaries
    QDateTime value; // date & time
    bool hasTime; // false if date only was in file
    bool hasTimeZone; // record contains TZ info
    short zoneHour, zoneMin; // TZ value
    enum DateFormat {
        ISOBasic,    // basic ISO 8601 format (in general, for vCard 2.1)
        ISOExtended, // extended ISO 8601 format (in general, for vCard 3.0)
        Local        // human-readable, according current locale
    };
    DateItem();
    DateItem(const QDateTime& dateTime);
    bool operator ==(const DateItem& d) const;
    void clear();
    QString toString(DateFormat format=DateItem::Local) const;
    inline bool isEmpty() const {return value.isNull(); }
    // "Little alien" functions for this structure:
    static QDateTime readISOExtDateTimeWithZone(const QString& src);
    static QString writeISOExtDateTimeWithZone(const QDateTime& src);
};

struct PostalAddress: public TypedDataItem {
    QString offBox, extended, street, city, region, postalCode, country;
    bool operator ==(const PostalAddress& a) const;
    void clear();
    virtual QString toString(bool humanReadable) const;
    static PostalAddress fromString(const QString& src, const QStringList& _types);
    bool isEmpty() const;
    static class StandardTypes: public ::StandardTypes {
        public:
        StandardTypes();
        void fill();
    } standardTypes;
};

struct Photo {
    QString pType; // URL, JPEG, PNG or unsupported, but stored value
    QByteArray data;
    QString url;
    bool operator ==(const Photo& p) const;
    void clear();
    bool isEmpty() const;
    QString detectFormat() const;
};

struct ContactItem {
    QString fullName;
    QStringList names;
    QList<Phone> phones;
    QList<Email> emails;
    DateItem birthday;
    DateItem anniversary;
    QString sortString;
    QString description;
    Photo photo;
    QStringList groups;
    // Work
    QString organization, title, role;
    // TODO logo?
    // Addresses
    QList<PostalAddress> addrs;
    // Internet
    QString nickName, url;
    QList<Messenger> ims;
    // Format internals
    QString id; // optional record unique id (udx Sequence, UID, vcf X-IRMC-LUID, etc)
    QString idType; // tag name    
    QString originalFormat;
    QString version, subVersion;
    DateItem lastRev;
    TagList otherTags;   // Known but un-editing tags
    TagList unknownTags; // specific tags for any file format, i.e. vcf
    // Calculated fields for higher perfomance
    QString visibleName, prefPhone, prefEmail, prefIM,
        allPhones, homePhone, workPhone, cellPhone;
    // Calculated fields for list comparison
    enum PairState {
        PairNotFound,
        PairSimilar,
        PairIdentical
    } pairState;
    ContactItem* pairItem;
    int pairIndex;
    // Calculated fields for hard sorting
    QString actualSortString; // Can be sortString, name(s), nick, depends on settings
    // Calculated tempory fields for call history update
    QString prevFullName;
    // Editing
    void clear();
    bool swapNames();
    bool splitNames(); // TODO int index
    bool joinNames();
    bool dropSlashes();
    bool intlPhonePrefix(int countryRule);
    // Aux methods
    void calculateFields(); // For perfomance
    template<class T>
    void sortTypes(QList<T> &values); // Type sorting and lowercasing for correct compare
    QString formatNames() const;
    QString makeGenericName() const;
    void parseFullName();
    void reverseFullName();
    void dropFinalEmptyNames(); // If empty parts not in-middle, remove it
    void formatPhones(const QString& templ);
    bool similarTo(const ContactItem& pair, int priorityLevel);
    bool identicalTo(const ContactItem& pair);
    static QString nameComponent(int compNum);
    const QString findIMByType(const QString& itemType) const;
    bool operator <(const ContactItem& pair) const;
};

// Specific data for backup files (MPB, NBF, NBU)
struct CallInfo {
    QString cType, timeStamp, duration, number, name;
#ifdef WITH_CALLS
    QString typeName() const;
#endif
};

#ifdef WITH_MESSAGES
struct BinarySMS { //primarilly for Nokia Prefdef messages
    QString name;
    QByteArray content;
};
#endif

struct ExtraData {
    // Common fields
    QString model;
    QDateTime timeStamp;
    QStringList vmsgSMS, pduSMS; // some file formats may contains both SMS type sets
    // MPB specific
    QStringList organizer, notes;
    QStringList vmsgSMSArchive, pduSMSArchive;
    QList<CallInfo> calls;
    // NBU specific
    QString imei, firmware, phoneLang;
    // NBU + NBF
#ifdef WITH_MESSAGES
    QList<BinarySMS> binarySMS;
#endif
    void clear();
};

// Entire address book
class ContactList : public QList<ContactItem>
{
public:
    ContactList();
    // Editing
    enum SortType {
        SortBySortString,
        SortByLastName,
        SortByFirstName,
        SortByFullName,
        SortByNick,
        SortByGroup
    };
    void clear();
    void sort(SortType sortType);
    void compareWith(ContactList& pairList);
    // Group operations
    QMap<QString, int> groupStat() const;
    bool hasGroups() const;
    bool hasGroup(const QString& group) const; // Call this before add/rename group!
    bool addGroup(const QString& group);
    bool renameGroup(const QString& oldName, const QString& newName);
    bool removeGroup(const QString& group);
    QStringList contactsInGroup(const QString& group);
    void includeToGroup(const QString& group, ContactItem& item);
    void excludeFromGroup(const QString& group, ContactItem& item);
    void mergeGroups(const QString& unitedGroup, const QString& mergedGroup);
    void splitGroup(const QString& existGroup, const QString& newGroup, const QList<int>& movedIndicesInGroup);
    // Non-standard tag cleaning
    QMap<QString, int> nonStandardTagUsage() const;
    bool massTagRemove(const QStringList& tagNames);
    // Info
    int findById(const QString& idValue) const;
    QString statistics() const;
#ifdef WITH_MESSAGES
    bool hasMessages();
#endif
#ifdef WITH_CALLS
    bool hasCalls();
#endif
    void updateCallHistory(const QStringList& droppedFullNames = QStringList());
    ExtraData extra;
    QStringList emptyGroups;
    QString originalPath; // for append-only formats, such as NBF
    QString originalProfile; // for CSV; see also ContactItem::originalFormat
    // Calculated
    int photoURLCount;
};

// Helper class for binding names to phone numbers
// TODO maybe make analytic reports based on it
class NumberNameMap: public QMap<QString,QString> {
public:
    NumberNameMap(const ContactList &list);
    QString nameByNumber(const QString& number);
};

#endif // CONTACTLIST_H
