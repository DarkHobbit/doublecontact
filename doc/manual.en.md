# DoubleContact Manual #

## About ##

DoubleContact is offline DE-independent contact manager primarily for phonebooks editing/merging.
DoubleContact UI is similar to two panel file manages, but second panel can be disabled.

Features:

* saving and loading VCF (vCard 2.1 and 3.0 supported, VCF files directory as single address book also supported),  UDX (Philips Xenium), MPB (MyPhoneExplorer backup);
* contact view in table, sorting and filtering supported;
* two panel view of two address books, with copy/move features;
* single record editing;
* group operations under selected records:
  - firstname/lastname swap;
  - first/last name splitting;
  - phone number internationalization for some countries (in general, for ex-USSR);
  - contact splitting for SIM card export (one phone per contact)
  - contact merge;
  -drop slashes and other specials;
  - formatted name generation;
* two address books comparison with highlighting of identical, similar and differ contacts;
* interactive merge according to the comparison results.

## Loading ##

Before loading, you must prepare address book file(s).

For example, Android phones allows save its address book in single VCF file (**Contacts -> Import/Export -> Export contacts to: -> SD card** in device menu). Depending on the Android version and modification, menu items may slightly differ from it. Copy result file from memory card to PC.

You can get similar file from hardware phones - SonyEricsson and some other vendors. Special case - Philips Xenium phones: it use UDX, its own format. DoubleContact also support this format.

Some phones and devices (for example, some Nokia PC Suite versions) save each contact in separate VCF file. In this case copy entire directory to PC.

If address book was saved in single file, select **File -> Open -> File** in DoubleContact menu and open received file with VCF, UDX or MPB extension. If address book is a directory, select **File -> Open -> Directory**.

In both cases, DoubleContact load address book and show it as a table.

The same way, you can load from MyPhoneExplorer backup file (MPB extension). In this case, DoubleContact load not only contacts, but also call history, organizer, notes and SMS. DoubleContact not support this extra data, but allow to save it in other MPB file after contacts edit.

Known MyPhoneExplorer bug: if NOTE field (contact description) contains line feed, it saved in MPB with false ENCODING=QUOTED-PRINTABLE attribute, and this cause note corruption.

If you got load problems (in most cases, unknown tags), DoubleContact show log windows with problem list. Don't panic! Unknown tags are accurately preserved during view/editing, and program will save it in output file. Furthermore, you can view its content at "Problems" tab of contact edit dialog. Despite this, you are very help to the author, if send him such tas samples.

## Saving ##

Before saving, you must define, for which device or program you write an address book. Most modern devices support vCard 3.0 or 4.0. But some phones requires vCard 2.1. You can set vCard version in settings window ( **View -> Settings** menu item, "Saving" tab). If you check "Use original file version (if present)" option, DoubleContact will try use vCard version from original file. Otherwise, program always will use version from Settings window.

In some cases, for example, for SIM cards, each contact must not contain more than one phone number. In this case, before saving you must split multi-phone contacts. Select splitting items (usually, the best way is select all) and select **Contact -> Split** menu item.

Some devices don't recognize birthdays, containing time. In this case, before saving you must activate time skipping ( **View -> Settings** menu item, "Saving" tab, set "Skip time from birthday and anniversaries" option).

To save address book to single file, select **File -> Save as -> File** in DoubleContact menu, select type (VCF, UDX or MPB) and enter file name. To save address book to VCF directory, select **File -> Save as -> Directory**. Now you can copy result file (directory) on device and import it.

DoubleContact allows save to MPB file **only** if original file also had MPB format. The reason is that MPB file contains  not only contacts, but also call history, organizer, notes and SMS (VCF and UDX not support this data). If save MPB to VCF, an then VCF to MPB, extra data will be loss!

## Contact view ##

DoubleContact show loaded contacts as a table. Each table row represents one contact.

You can change column visibility and order ( **View -> Settings** menu item, "Columns" tab). Simply move needed columns from "Available" to "Visible" list, using arrow buttons. Most column names are trivial ("Firstname", "Lastname", etc.), but there are exceptions.

"Phone" column contains phone number with "Preferable" type, or, if it missing, first phone number of contact. "Email" column work similar.

Some columns are flags: it show \* character, if any condition is true. So, condition "Has phone" is true, if contact has at least one phone number, "Some phones" - if if contact has **more** than one phone number. Same flags are for emails and birthdays.

"Generic name" is designed to show contact info even contact entered badly. If full (formatted) name is exists, it shown in this column. Otherwise, program try to construct it from names. If names are empty, program try use organization name, note, phone or email, and, at last, sort string.














