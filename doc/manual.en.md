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
  - first/last name swap;
  - first/last name splitting;
  - phone number internationalization for some countries (in general, for ex-USSR);
  - contact splitting for SIM card export (one phone per contact)
  - contact merge;
  - drop slashes and other specials;
  - formatted name generation;
* two address books comparison with highlighting of identical, similar and differ contacts;
* interactive merge according to the comparison results.

## Loading ##

Before loading, you must prepare address book file(s).

For example, Android phones allows save its address book in single VCF file (**Contacts -> Import/Export -> Export contacts to: -> SD card** in device menu). Depending on the Android version and modification, menu items may slightly differ from it. Copy result file from memory card to PC.

You may get similar file from hardware phones - SonyEricsson and some other vendors. Special case - Philips Xenium phones: it use UDX, its own format. DoubleContact also support this format.

Some phones and devices (for example, some Nokia PC Suite versions) save each contact in separate VCF file. In this case copy entire directory to PC.

If address book was saved in single file, choose **File -> Open -> File** in DoubleContact menu and open received file with VCF, UDX or MPB extension. If address book is a directory, choose **File -> Open -> Directory**.

In both cases, DoubleContact load address book and show it as a table.

The same way, you can load from MyPhoneExplorer backup file (MPB extension). In this case, DoubleContact load not only contacts, but also call history, organizer, notes and SMS. DoubleContact not support this extra data, but allow to save it in other MPB file after contacts edit.

Known MyPhoneExplorer bug: if NOTE field (contact description) contains line feed, it saved in MPB with false ENCODING=QUOTED-PRINTABLE attribute, and this cause note corruption.

If you got load problems (in most cases, unknown tags), DoubleContact show log windows with problem list. Don't panic! Unknown tags are accurately preserved during view/editing, and program will save it in output file. Furthermore, you can view its content at "Problems" tab of contact edit dialog. Despite this, you are very help to the author, if send him such tas samples.

## Saving ##

Before saving, you must define, for which device or program you write an address book. Most modern devices support vCard 3.0 or 4.0. But some phones requires vCard 2.1. You can set vCard version in settings window ( **View -> Settings** menu item, "Saving" tab). If you check "Use original file version (if present)" option, DoubleContact will try use vCard version from original file. Otherwise, program always will use version from Settings window.

In some cases, for example, for SIM cards, each contact must not contain more than one phone number. In this case, before saving you must split multi-phone contacts. Select splitting items (usually, the best way is select all) and choose **Contact -> Split** menu item.

Some devices don't recognize birthdays, containing time. In this case, before saving you must activate time skipping ( **View -> Settings** menu item, "Saving" tab, set "Skip time from birthday and anniversaries" option).

To save address book to single file, choose **File -> Save as -> File** in DoubleContact menu, select type (VCF, UDX or MPB) and enter file name. To save address book to VCF directory, choose **File -> Save as -> Directory**. Now you can copy result file (directory) on device and import it.

DoubleContact allows save to MPB file **only** if original file also had MPB format. The reason is that MPB file contains  not only contacts, but also call history, organizer, notes and SMS (VCF and UDX not support this data). If save MPB to VCF, an then VCF to MPB, extra data will be loss!

## Contact view ##

DoubleContact show loaded contacts as a table. Each table row represents one contact.

You can change column visibility and order ( **View -> Settings** menu item, "Columns" tab). Simply move needed columns from "Available" to "Visible" list, using arrow buttons. Most column names are trivial ("Firstname", "Lastname", etc.), but there are exceptions.

"Phone" column contains phone number with "Preferable" type, or, if it missing, first phone number of contact. "Email" column work similar.

Some columns are flags: it show \* character, if any condition is true. So, condition "Has phone" is true, if contact has at least one phone number, "Some phones" - if if contact has **more** than one phone number. Same flags are for emails and birthdays.

"Generic name" is designed to show contact info even contact entered badly. If full (formatted) name is exists, it shown in this column. Otherwise, program try to construct it from names. If names are empty, program try use organization name, note, phone or email, and, at last, sort string.

Sorting switched on/off by F4 key. Click on any column header to set sort order.

By default, all contacts are visible. To activate fitering, press Ctrl+F. Focus will moved to text field under table. If you enter name of name part here, only records, containing this part, will be visible. For example, enter "Ja" - contacts "James Bond", "George Jackson" will be shown. Enter "213", and contact with "+1-213-555-1234" phone will be shown. If you clear filter field, all contacts will be visible again. Note: filtering work only by visible column. For example, if you remove "Phone" from visible columns, my second example will not work.

Choose **View -> Two panels** to switch on two panel mode. Separated address books can be loaded in ech panel. Press Tab or click on opposite panel to change current panel. Press Ctrl+U to swap panel content.

To copy one or some records from one loaded address book to other, select this records and press F5. To move - select and press F6 (in last case, selected records will be deleted from source).

Choosing **View -> Two panels** again  will turn off two panel mode.

On top of each address book, DoubleContact show source file (directory) name of this address book. If it has unsaved changes, name is accompanied by a  (\*) character.

Choose **List -> Statistics** to view general address book info: total number of contacts, phone numbers, emails, etc. If address book was loaded from MPB file, you will see also number off SMS, calls, device model name and backup date.

## Editing ##

Main editing mode in DoubleContact is a single-record editing. Group editing also works, but in group editing only some fields can be edited. Single-record and group editing are using different edit windows.

To edit one contact, choose it and press "Edit". You also can press Enter key or double-click the contact item. DoubleContact will open an "Edit contact" dialog. It has a multi-tab view.

On "General" tab, total fields number depends of contact fulness: names - from 2 to 5, minimal phone numbers and emails - 1 (maximum not limited). General names order: last (family) name, first () name, additional names, honorific prefixes, and honorific suffixes. All fields are optional.

You can enter phone numbers and emails in free form. But if this fields contains invalid characters, your device, probably, will not recognize it.

To add new name, phone, or email, select information kind in list next to the "Add..." button and press "Add...". To remove one of this fields, press red-crossed button near field.

Entering phone, don't forget select its type: home, work, cellular, etc. You can want to combine this types (in most cases, one from simple types is combined with "preferable" type). In this case, select "Mixed...". Program will open "Phone type" window to let you make needed combinations by flags. Note that many devices treat "Voice" type as "Other".

Contact can contain a birthday. In most cases, birthday contains only date: set flag, enter day, month and year. But vCard standard allows also set a birthday time. To do it, press "Details" button. Program will open "Date details" window. In it, besides date, you can set time and even time zone for it.

Besides birthday, you can set other anniversary (in most cases, it's a marriage anniversary). Select "anniversary" in list near "Add..." button and press "Add..." button. Anniversary is edited as well as birthday.

If contact contains photo or photo URL, it's shown in "Photo" field. Under it, you can see self-titled button. Press this button to get menu with items:

* "Load image" - load photo from JPEG or PNG file. If contact already contains photo or photo URL, it will be removed;
* "Save image" - save photo to file of same format as contact source;
* "Set URL" - enter or edit photo URL in separate window. If contact already contains photo, it will be removed;
* "Remove photo" - remove current photo or photo URL from contact.

On "Addresses" tab, you can enter one or two addresses (home or/and work). If home and work addresses are confused, you can fix it pressing "<>" button. On same tab, you can set job title and organization name.

"Internet" is the simplest tab. Here you can enter nickname, site URL and accounts in various IMs.

"Other" tab contains technical internals. It can be used for bug report. Here you also can see vCard tags, known by DoubleContact, but not edited. Here you also can set string for contact sorting.

At last, "Problems" tab contains unknown tags (attributes) of contact. As mentioned above, unknown tags are preserved during view/editing, and program will save it in output file. Please, send such tag samples to author.

To save editing results, press "OK". Or save "Cancel" to reject changes.

You also can select some contact items and call contact editing. But in this mode, you can change only few fields: job title, organization, note.

To add new contact in current address book, press "Insert" in main window or Ins key. DoubleContact will open same window as for single editing.

## Group operations ##

Often first and last names in differ contacts of one address book are written in differ order (especially if address book is many year old). To fix it, select all items, where first and last names are confused (for example, last name is John, first name is Doe), and choose **Contact -> Swap names**. DoubleContact will exchange first and last name for all selected items.

Next operation, name splitting, is needed, if name parts was written in address book as single string. This case is typical for address books, imported from old device SIM card. Select such items and choose **Contact -> Split names** (Shift+F2). DoubleContact will split names by whitespaces. For some items, after it, you may have to swap names (as described above), if name parts was confused initially.

You may want join two items to one contact. In most cases, it's a two phones or one person, or him/her phone and email, etc. Select strongly two items on current panel and choose **Contact -> Join**. DoubleContact will open "Item Merge" window. In this window, equal and differ contact parts are highlighted by green and red. Use arrow buttons to copy data from one contact to other: names, phones, ermails, etc. After editing both contacts still saved in table. You can save most complete item and remove other manually.

If you selected greater or less than 2 items before join, it will cause error message.

Reverse operation, contact splitting by phones, is needed, if address book would be load to SIM card with no more than one phone per contact. Select splitting contacts (in most cases, simplest way is select all) and choose **Contact -> Split**.

Next group operation is autogeneration of full (formatted) name by entered names (**Contact -> Generate full name**). Full name generation, in particular, facilitates comparison of two address book, only one of which contains full names. DoubleContact also allows delete previously entered full names (**Contact -> Drop full name**) and swap name parts in it (**Contact-> Reverse full name**).

In some cases, if VCF file was imported from SIM card, it may contain special characters: contact names contains backslash, often accompanied with a digit. **Contact -> Drop slashes** command allows you remove this trash.

Last group operation is a phone numbers internationalization (only for some countries). For Belarus, this operation replaces prefix 8 to country code +375, for Russia - 8 to +7, for Ukraine - 0 to +380.

To internationalize phone numbers, select some contact ant choose **Contact -> Intl phone prefix**. DoubleContact prompts you to select a country. This function is implemented only for some countries. If you think that phone numbers internationalization is needed for your country, but the country is absent in list, please, contact author.

ATTENTION! Automatic country selection is impossible, because one national prefix may correspond to differ international prefixes for various countries. For example, 8 is converted to +7 for Russia, same 8 converted to +375 for Belarus. If you try internationalize all contacts at one pass, it may corrupt phone numbers.
Because, if address book contains both Russian and Belarusian phones in national forms, at first, select and internationalize Belarusian phones, then Russian, or vice versa. Of course, it's very rare case, but it can cause after merging of two address book, one of which initially created in Belarus, other in Russia. 


## Address book comparison and interactive contact merging ##

In two panel mode, DoubleContact allows compare two loaded address books semi-automatically.

To compare two address books, choose **List -> Compare** (F3). Program will search contact pairs in both books and switch to compare mode. After search finish, all contacts will be highlighted:

* unpaired contacts are highlighted in red;
* similar contacts are highlighted in yellow;
* identical contacts are highlighted in green.

Contacts are identical if it have identical names, full (formatted) names, phones, emails, birthdays, anniversaries, sot strings, notes, photos, organizations, job titles, site URLs and IM accounts. If some of these fields are empty, it must be empty in both contacts.

Checking for similar items is less trivial and use multi-pass algorithm. At first pass, DoubleContact checks items for identical phones and emails. If at least some of them are identical, contacts are similar. Thus, entire list would be checked. If pair not found, at next passes, program will check IDs, postal address, formatted names, standalone names, nicknames and organizations. Thus, checking will be performed on a more reliable criteria to less reliable. False positives can occur, we strongly recommend check such pairs manually.

If you select item on one panel in compare mode, and this item have a pair at other panel, pair item also will be selected. Choose **Contact -> Compare result** (Shift+F3) to open "Item merge" window. Identical fields will be highlighted in green, differ - in red. You can sync fields by arrow buttons (left, right).

Some sections, such as phones and emails, can have two kind of arrow buttons. Arrow inside section calls copying of separate phone (email), arrow outside section - copying of entire section.

**Example** : we have two contacts file. First, backup.vcf, is a full but outdated backup of broken device address book. Second, sim.vcf, imported from SIM card after placing it into new device. We must combine all valuable information from both files and save it to backup_new.vcf file.

At first, load backup.vcf into DoubleContact left panel. Load sim.vcf into right panel. Press F3. Some items at right panel will be highlighting in red. This means that they were added on SIM card after backup. Select these items and copy it ti left panel (F5). After it, for best contact view, use name swapping and name splitting, if needed (see chapter "Group operations").

You must not do anything with green items - these items are already OK. But yellow items requires your attention. For example, DoubleContact found pair with "John Doe" names. Select any contact in this pair and press F3. In "Item merge" window, names and addresses highlighted by green, but phones and birthday highlighted by red. There is a new phone number appear in right list. Copy it to left list by arrow button. Also, only left panel contains a birthday. Copy it to right (generally speaking, it is not necessary, if you want save only left address book, but will help avoid confusion). Press "Yes" to save changes.

After processing of all items, select left panel, choose **File -> Save as -> File**  and save the merged address book as backup_new.vcf.

After repeated **List -> Compare** (F3) choosing, DoubleContact will switched in simple view mode again.


## How to help ##

DoubleContact is a free software, distributed under the terms of the GNU General Public License, either version 2 of the License, or (at your option) any later version.

The first things you can help are bug reports and feature requests. DoubleContact bug tracker is placed at [GitHub](https://github.com/DarkHobbit/doublecontact/issues). If you can not work with the bug tracker, but found a critical error, send me at pub@zvyozdochkin.ru. Please, write on English or Russian.

If some items in your address books are highlighted in yellow in simple view mode, it means that they have unknown tags. Send me files with such tags. I guarantee that your files will be used **only** for bug search and fixes in a program.

If warning messages appears during address book loading or saving, you can save these messages via "Copy to clipboard" button and send it to me. Please, tell me name of device or program, in which file was created. It's desirable to attach address book file.

Now, author working on CSV (comma-separated values) format support in DoubleContact. There isn't standard agreement, how contact must be stored in CSV files. If you are interested in CSV support, send me example of CSV file, created by your device/program or its fragment. If you want sent a fragment only, please check if entire file a header (it means that one or some first lines of file contains column names and, probably other internal information instead contacts). If header is present, send it too.

The second thing you can help in development is translation to national languages. If you know a language, in which DoubleContact isn't translated - join now. TODO: url

## Tips and tricks ##

DoubleContact have some command-line options:

* --debugdata or -d - run program with test data set (some contacts on differ languages);
* --fullscreen or -f - run program in full screen mode;
* --quiet or -q - don't run any address books at startup, even if 











































