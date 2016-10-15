/* Double Contact
 *
 * Module: Global definitions
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QHash>
#include <QItemDelegate>
#include <QObject>
#include <QStringList>

// Visible columns
enum ContactColumn {
  ccFirstName, ccLastName, ccMiddleName, // names
  ccFullName, ccGenericName, // formatted names
  ccPhone,      // first or preferred phone
  ccEMail,      // first or preferred email
  ccBDay,       // birthday
  ccTitle,      // Job title
  ccOrg,        // Organization
  ccAddr,       // postal address
  ccHasPhone,   // contact has at least one phone
  ccHasEmail,   // contact has at least one phone
  ccHasBDay,    // contact has birthday
  ccSomePhones, // contact has more than one phone
  ccSomeEmails, // contact has more than one email
  ccLast
};

// Common strings for translation unify
// Message boxes
#define S_ERROR QObject::tr("Error")
#define S_WARNING QObject::tr("Warning")
#define S_CONFIRM QObject::tr("Confirmation")
#define S_INFORM QObject::tr("Information")
// Misc
#define S_NEW_LIST QObject::tr("New contact list")
#define S_READ_ONLY_FORMAT QObject::tr("This format is read only")
// Spec.value for combined phone/mail/addr. types
#define S_MIXED_TYPE QObject::tr("mixed...")
// Column names
#define S_FIRST_NAME QObject::tr("Firstname")
#define S_LAST_NAME QObject::tr("Lastname")
#define S_MIDDLE_NAME QObject::tr("Middlename")
#define S_FULL_NAME QObject::tr("Full name")
#define S_GENERIC_NAME QObject::tr("Generic name")
#define S_PHONE QObject::tr("Phone")
#define S_EMAIL QObject::tr("Email")
#define S_BDAY QObject::tr("Birthday")
#define S_TITLE QObject::tr("Job title")
#define S_ORG QObject::tr("Organization")
#define S_ADDR QObject::tr("Address")
#define S_HAS_PHONE QObject::tr("Has phone")
#define S_HAS_EMAIL QObject::tr("Has email")
#define S_HAS_BDAY QObject::tr("Has birthday")
#define S_SOME_PHONES QObject::tr("Some phones")
#define S_SOME_EMAILS QObject::tr("Some emails")
// Address components
#define S_ADR_OFFICE_BOX QObject::tr("P.o. box")
#define S_ADR_EXTENDED QObject::tr("Ext.addr.")
#define S_ADR_STREET QObject::tr("Street")
#define S_ADR_CITY QObject::tr("City/Locality")
#define S_ADR_REGION QObject::tr("Region")
#define S_ADR_POST_CODE QObject::tr("Postal code")
#define S_ADR_COUNTRY QObject::tr("Country")
// Photo editing
#define S_PH_UNKNOWN_FORMAT QObject::tr("[Unknown photo format]")
#define S_PH_LOAD_IMAGE QObject::tr("Load image")
#define S_PH_SAVE_IMAGE QObject::tr("Save image")
#define S_PH_SET_URL QObject::tr("Set URL")
#define S_PH_REMOVE QObject::tr("Remove photo")
// File ops
#define S_ALL_SUPPORTED QObject::tr("All supported files (%1)")
#define S_ALL_FILES QObject::tr("All files (*.*)")
#define S_READ_ERR QObject::tr("Can't read file\n%1")
#define S_WRITE_ERR QObject::tr("Can't write file\n%1")

// Visible columns headers
// (It was a simple string array, but its translation not worked)
class ContactColumnHeaders: public QStringList
{
public:
    ContactColumnHeaders();
    void fill();
};

extern ContactColumnHeaders contactColumnHeaders;

class ContactColumnList: public QList<ContactColumn>
{};

// Standart support types (phone, email, address) storage and checker
class StandardTypes: public QHash<QString, QString> {
    public:
    // untranslated keys stored in lowercase
    QString translate(const QString& key, bool* isStandard = 0) const;
    QString unTranslate(const QString& value) const;
    QStringList displayValues;
};

// Delegate for making table widgets read only
// Thanx to Axis - http://axis.bplaced.net/news/460
class ReadOnlyTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ReadOnlyTableDelegate(QObject* parent = 0);
    virtual QWidget* createEditor(
        QWidget*, const QStyleOptionViewItem&, const QModelIndex &) const;
};

extern
struct GlobalConfig {
    QString dateFormat, timeFormat;
    enum VCFVersion {
        VCF21,
        VCF30
        //VCF40
    } preferredVCFVersion;
    bool useOriginalFileVersion;
} gd;

#endif // GLOBALS_H
