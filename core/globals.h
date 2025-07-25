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
#include <QObject>
#include <QStringList>
#include <qnamespace.h>

#include "corehelpers.h"

// Visible columns
enum ContactColumn {
  ccFirstName, ccLastName, ccMiddleName, // names
  ccFullName, ccGenericName, // formatted names
  ccPhone,      // first or preferred phone
  ccAllPhones,
  ccHomePhone,
  ccWorkPhone,
  ccCellPhone,
  ccEMail,      // first or preferred email
  ccBDay,       // birthday
  ccGroups,     // all groups
  ccTitle,      // Job title
  ccOrg,        // Organization
  ccRole,
  ccAddr,       // postal address
  ccNickName,   // nickname
  ccUrl,        // site url
  ccIM,         // IMs
  ccIMJabber,
  ccIMICQ,
  ccIMSkype,
  ccLastRev,    // last vCard update (REV)
  ccVersion,    // vCard version (VERSION)
  ccHasPhone,   // contact has at least one phone
  ccHasEmail,   // contact has at least one phone
  ccHasAddress, // contact has at least one postal address
  ccHasBDay,    // contact has birthday
  ccHasPhoto,   // contact has at least one photo
  ccHasProblems,// contact has at least one unknown tag
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
#define S_ADDRESS_BOOK QObject::tr("Address book")
#define S_NEW_LIST QObject::tr("New contact list")
// Dialogs
#define S_GROUP_NAME QObject::tr("Group name")
#define S_COUNTRY_RULE_SELECT QObject::tr("Select country rule")
#define S_COUNTRY_PHONE_TEMPLATE QObject::tr("Select phone template")
#define S_TRIM_NAMES QObject::tr("Trim names")
#define S_TN_MAXNAMES QObject::tr("Maximum name components count")
#define S_TN_MAXLEN QObject::tr("Maximum name component length")
#define S_TN_TOTALLEN QObject::tr("Total maximum name length: %1")
#define S_MASS_TAG_REMOVE QObject::tr("Select tags for remove")
#define S_CSV_PROFILE_SELECT QObject::tr("Select CSV profile")
#define S_GENERIC_CSV_PROFILE QObject::tr("Generic profile")
// Spec.value for combined phone/mail/addr. types
#define S_MIXED_TYPE QObject::tr("mixed...")
// Name components
#define S_FIRST_NAME QObject::tr("Firstname")
#define S_LAST_NAME QObject::tr("Lastname")
#define S_MIDDLE_NAME QObject::tr("Middlename")
#define S_NAME_PREFIXES QObject::tr("Prefixes") // Honorific prefixes
#define S_NAME_SUFFIXES QObject::tr("Suffixes") // Honorific suffixes
// Other column names
#define S_FULL_NAME QObject::tr("Full name")
#define S_GENERIC_NAME QObject::tr("Generic name")
#define S_DESC QObject::tr("Description")
#define S_PHONE QObject::tr("Phone")
#define S_ALL_PHONES QObject::tr("All phones")
#define S_HOME_PHONE QObject::tr("Home phone")
#define S_WORK_PHONE QObject::tr("Work phone")
#define S_CELL_PHONE QObject::tr("Cell phone")
#define S_EMAIL QObject::tr("Email")
#define S_BDAY QObject::tr("Birthday")
#define S_ANN QObject::tr("Anniversary")
#define S_PHOTO QObject::tr("Photo")
#define S_TITLE QObject::tr("Job title")
#define S_ORG QObject::tr("Organization")
#define S_ROLE QObject::tr("Role")
#define S_ADDR QObject::tr("Address")
#define S_NICK QObject::tr("NickName")
#define S_URL QObject::tr("URL")
#define S_IM QString("IM")
#define S_IM_JABBER QObject::tr("Jabber")
#define S_IM_ICQ QObject::tr("ICQ")
#define S_IM_SKYPE QObject::tr("Skype")
#define S_GROUP QObject::tr("Group")
#define S_LAST_REV QObject::tr("Updated")
#define S_VERSION QObject::tr("Ver.")
// Columns-flags
#define S_HAS_PHONE QObject::tr("Has phone")
#define S_HAS_EMAIL QObject::tr("Has email")
#define S_HAS_ADDRESS QObject::tr("Has address")
#define S_HAS_BDAY QObject::tr("Has birthday")
#define S_HAS_PHOTO QObject::tr("Has photo")
#define S_HAS_PROBLEMS QObject::tr("Has problems")
#define S_SOME_PHONES QObject::tr("Some phones")
#define S_SOME_EMAILS QObject::tr("Some emails")
// Address components
#define S_ADR_OFFICE_BOX QObject::tr("P.O. box")
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
#define S_PH_SHOW_IN_NEW_WIN QObject::tr("Show in new window")
#define S_PH_SET_URL QObject::tr("Set URL")
#define S_PH_REMOVE QObject::tr("Remove photo")
// File ops
#define S_ALL_SUPPORTED QObject::tr("All supported files (%1)")
#define S_ALL_FILES QObject::tr("All files (*.*)")
// Common errors, warnings and questions
#define S_READ_ERR QObject::tr("Can't read file\n%1")
#define S_WRITE_ERR QObject::tr("Can't write file\n%1")
#define S_MKDIR_ERR QObject::tr("Can't create directory\n%1")
#define S_SEEK_ERR QObject::tr("Can't seek to %1 in file\n%2")
#define S_REC_NOT_SEL QObject::tr("Record not selected")
#define S_ONLY_ONE_REC QObject::tr("Group operation not implemented, select one record")
#define S_READ_ONLY_FORMAT QObject::tr("This format is read only")
#define S_ERR_UNSUPPORTED_TAG \
    QObject::tr("Warning: contact %1 has %2, not supported in this format.\nData will be lost")
#define S_ERR_EXTRA_TAG \
    QObject::tr("%1 %2 will be lost at contact %3")
#define S_ERR_NETWORK_SUPPORT \
    QObject::tr("Program built without network support. Use WITH_NETWORK define, if building from source code.\n")
#define S_REMOVE_CONFIRM QObject::tr("Do you really want to delete the selected items?")
#define S_SORTMASK_CONFIRM QObject::tr("Column sorting is switched. Hard sort results are not visible.\nAre you want switch column sorting off?")
#define S_LAST_SECTION QObject::tr("Last section not closed")
#define S_UNKNOWN_ENCODING QObject::tr("Unknown encoding: %1")
#define S_UNKNOWN_CHARSET QObject::tr("Unknown charset: %1")
// Status messages
#define SS_MODE QObject::tr("Mode: ")
#define SS_SORT_ON QObject::tr("sorted")
#define SS_SORT_OFF QObject::tr("not sorted")
// SMS/MMS messages
#define S_UNKNOWN_MSG_TAG QObject::tr("Unknown vMessage tag: %1")
#define S_UNKNOWN_MSG_VAL QObject::tr("Unknown vMessage value: %1")
#define S_SELECT_MMS_DIR_TITLE QObject::tr("Select MMS files Directory")
// Model item roles
#define SortStringRole Qt::UserRole

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

extern
struct GlobalConfig {
    // Misc
    bool openLastFilesAtStartup;
    // View
    bool showTableGrid;
    bool showLineNumbers;
    bool resizeTableRowsToContents;
    bool useTableAlternateColors;
    bool useSystemFontsAndColors;
    QString tableFont, gridColor1, gridColor2;
    // Locale
    QString dateFormat, timeFormat;
    bool useSystemDateTimeFormat;
    // Columns
    ContactColumnList columnNames;
    // Save
    enum VCFVersion {
        VCF21,
        VCF30,
        VCF40
    } preferredVCFVersion;
    bool useOriginalFileVersion;
    bool writeFullNameIsEmpty;
    int defaultCountryRule; // for phone i18n during compare numbers (i.e. for Russia +7 = 8)
    bool skipTimeFromDate;
    // addXToNonStandardTypes and nltnReplaceToDefault is standard behaviour
    // of some vCard2.1-based addressbooks (LG Leon)
    // Samsung A50 uses nltnUseXCustom
    bool addXToNonStandardTypes;
    enum NonLatinTypeNamesPolicy { // What Shall We Do with... non-latin phonetypenames
        nltnSaveAsIs,         // when we saving it in vCard structures
        nltnReplaceToDefault, // replace to any tag TODO make it tunnable
        nltnUseXCustom        // use X-CUSTOM parameter
    } nonLatinTypeNamesPolicy;
    enum GroupFormat {
        gfCategories = 0,
        gfXGroupMembership = 1, // Honor/Huawei
        gfXOppoGroups = 2,      // Realme/OPPO
        // gfXCategories,   // x-categories used by Nokia suite according Anisimov's article
        // Auto
        // Next options are for specific file format and don't selectable by user
        gfNBF,
        gfMPB
    } groupFormat;
    QString saveCharSet; // not all formats!
    // Load
    QString defaultEmptyPhoneType; // if phone w/o type was in loaded file
    bool warnOnMissingTypes;
    bool warnOnNonStandardTypes;
    bool readNamesFromFileName;
    bool debugSave;
    // Session-specific data from command line
    bool fullScreenMode; // Maximize main window at startup
    bool debugDataMode; // Show debug data at startup
    bool quietMode; // Don't load any files
    QStringList startupFiles;
} gd;

// Enumerations
extern EnumSetting enPrefVCFVersion, enNlTnPolicy, enGroupFormat;

#endif // GLOBALS_H
