/* Double Contact
 *
 * Module: Contact book visualization model
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <algorithm>
#include <QBrush>
#include <QFileInfo>
#include <QMimeData>
#include <QTextStream>

#include "contactmodel.h"
#include "modelhelpers.h"
#include "formats/common/vcarddata.h"
#include "formats/files/vcfdirectory.h"

ContactModel::ContactModel(QObject *parent, const QString& source, RecentList& recent) :
    QAbstractTableModel(parent), _source(source), _sourceType(ftNew),
    _changed(false), _viewMode(ContactModel::Standard), _recent(recent)
{
    // Default visible columns
    visibleColumns.clear();
    visibleColumns.push_back(ccLastName);
    visibleColumns.push_back(ccFirstName);
    visibleColumns.push_back(ccPhone);
}

ContactModel::~ContactModel()
{
}

QString ContactModel::source()
{
    return _source;
}

FormatType ContactModel::sourceType()
{
    return _sourceType;
}

bool ContactModel::changed()
{
    return _changed;
}

void ContactModel::updateVisibleColumns()
{
    beginResetModel();
    visibleColumns.clear();
    foreach (const ContactColumn& col, gd.columnNames)
        visibleColumns.push_back(col);
    endResetModel();
}

Qt::ItemFlags ContactModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    if (index.isValid())
        f = f | Qt::ItemIsDragEnabled;
    return f;
}

int ContactModel::rowCount(const QModelIndex &) const
{
    return items.count();
}

int ContactModel::columnCount(const QModelIndex &) const
{
    return visibleColumns.count();
}

QVariant ContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((role == Qt::DisplayRole) && (orientation==Qt::Horizontal))
        return contactColumnHeaders[visibleColumns[section]];
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ContactModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
          return QVariant();
      if (index.row() >= items.count())
          return QVariant();
    const ContactItem& c = items[index.row()];
    if (role==Qt::DisplayRole) {
        // Detect column
        ContactColumn col = visibleColumns[index.column()];
        switch (col) {
            case ccLastName:    return !c.names.isEmpty() ? c.names[0] : QVariant();
            case ccFirstName:   return c.names.count()>1  ? c.names[1] : QVariant();
            case ccMiddleName:  return c.names.count()>2  ? c.names[2] : QVariant();
            case ccFullName:    return c.fullName;
            case ccGenericName: return c.visibleName; // must be calculated
            case ccPhone:       return c.prefPhone;
            case ccAllPhones:   return c.allPhones;
            case ccHomePhone:   return c.homePhone;
            case ccWorkPhone:   return c.workPhone;
            case ccCellPhone:   return c.cellPhone;
            case ccEMail:       return c.prefEmail;
            case ccBDay:        return c.birthday.toString(DateItem::Local);
            case ccGroups:      return c.groups.join(", ");
            case ccTitle:       return c.title;
            case ccOrg:         return c.organization;
            case ccRole:         return c.role;
            case ccAddr:  {
                QString res = "";
                foreach (const PostalAddress& addr, c.addrs) {
                    QString sAddr = addr.toString(true);
                    if (!sAddr.isEmpty()) {
                        if (!res.isEmpty())
                            res += "; ";
                        res += sAddr;
                    }
                }
                return res;
            }
            case ccNickName:    return c.nickName;
            case ccUrl:         return c.url;
            case ccIM:          return c.prefIM;
            case ccIMJabber:    return c.findIMByType("xmpp");
            case ccIMICQ:       return c.findIMByType("icq");
            case ccIMSkype:     return c.findIMByType("skype");
            case ccLastRev:     return c.lastRev.toString();
            case ccVersion:     return c.version;
            case ccHasPhone:    return !c.phones.isEmpty() ? "*" : QVariant();
            case ccHasEmail:    return !c.emails.isEmpty() ? "*" : QVariant();
            case ccHasAddress:  return !c.addrs.isEmpty() ? "*" : QVariant();
            case ccHasBDay:     return !c.birthday.isEmpty() ? "*" : QVariant();
            case ccHasPhoto:    return !c.photo.isEmpty() ? "*" : QVariant();
            case ccHasProblems: return !c.unknownTags.isEmpty() ? "*" : QVariant();
            case ccSomePhones:  return c.phones.count()>1  ? "*" : QVariant();
            case ccSomeEmails:  return c.emails.count()>1  ? "*" : QVariant();
            case ccLast: { return QVariant(); } // Boundary case
            default: return QVariant();
        }
    }
    else if (role==Qt::BackgroundRole) {
        switch (_viewMode) {
        case ContactModel::Standard:
        case ContactModel::StandardWithQuickView:
            return (c.unknownTags.isEmpty()) ? QVariant() : QBrush(Qt::yellow);
        case ContactModel::CompareOpposite:
        case ContactModel::CompareMain:
            return(c.pairState==ContactItem::PairNotFound ? QBrush(Qt::red) :
                (c.pairState==ContactItem::PairIdentical ? QBrush(Qt::green) :
                    (c.pairState==ContactItem::PairSimilar ? QBrush(Qt::yellow) : QVariant())));
        case ContactModel::DupSearch:
            // TODO
            break;
        }
    }
    else if (role==SortStringRole) {
        ContactColumn col = visibleColumns[index.column()];
        if (col==ccBDay) // Dates must be sort by year, not by day
            return c.birthday.toString(DateItem::ISOBasic);
        else
            return emptyItemsToEnd(data(index, Qt::DisplayRole));
    }
    return QVariant();
}

Qt::DropActions ContactModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList ContactModel::mimeTypes() const
{
    return QStringList() << "text/vcard";
}

QMimeData *ContactModel::mimeData(const QModelIndexList &indexes) const
{
    VCardData d;
    BStringList lines;
    QStringList errors;
    QMimeData *mimeData = new QMimeData();
    foreach (const QModelIndex &index, indexes)
    if (index.isValid() && index.column()==0)
    {
        const ContactItem& c = items[index.row()];
        d.exportRecord(lines, c, errors);
    }
    mimeData->setData(mimeTypes()[0], lines.joinByLines());
    return mimeData;
}

bool ContactModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
      int, int column, const QModelIndex& index)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("text/vcard"))
        return false;
    if (column > 0)
         return false;
    // TODO here m.b. read other format (probably, text)
    BStringList encodedData = BString(data->data("text/vcard")).splitByLines();
    QStringList errors;
    VCardData d;
    beginResetModel();
    if (index.row()==-1)
        d.importRecords(encodedData, items, true, errors);
    else {
        ContactList addition;
        d.importRecords(encodedData, addition, false, errors);
        for(int i=0; i<addition.count(); i++)
            items.insert(index.row()+i, addition[i]);
    }
    endResetModel();
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory();
#endif
    // TODO insert to pointed line
    return (action == Qt::CopyAction || action == Qt::MoveAction);
}

bool ContactModel::removeRows(int row, int count, const QModelIndex&)
{
    QStringList droppedFullNames;
    beginRemoveRows (QModelIndex(), row, row+count-1);
    for (int i=row+count-1; i>=row; i--) {
        droppedFullNames << items[i].fullName;
        items.removeAt(i);
    }
    endRemoveRows();
    _changed = true;
    return false;
}

bool ContactModel::open(const QString& path, FormatType fType, QStringList &errors, QString &fatalError)
{
    if (path.isEmpty()) return false;
    // Add _old_ name to recent
    if (!_source.isEmpty() && _sourceType!=ftNew)
        _recent.addItem(_source);
    FormatType realType = fType;
    QString realPath = path;
    if (fType==ftAuto) {
        // TODO m.b. here add network protocol
        if (path.startsWith("file://"))
            realPath.remove("file://");
        realType = QFileInfo(realPath).isDir() ? ftDirectory : ftFile;
    }
    IFormat* format = 0;
    switch (realType) {
    case ftFile:
        format = factory.createObject(realPath);
        break;
    case ftDirectory:
        format = new VCFDirectory();
        break;
    default: // Only to avoid warning :(
        break;
    }
    if (!format) {
        fatalError = factory.error;
        return false;
    }
    if (!checkForCSVProfile(format, "")) {
        fatalError = format->fatalError();
        delete format;
        return false;
    }
    beginResetModel();
    bool res = format->importRecords(realPath, items, false);
    fatalError = format->fatalError();
    errors = format->errors();
    delete format;
    endResetModel();
    if (!res)
        return false;
    _source = path;
    _sourceType = realType;
    _changed = false;
    _recent.removeItem(path);
    return true;
}

bool ContactModel::saveAs(const QString& path, FormatType fType, QStringList &errors, QString &fatalError)
{
    if (path.isEmpty()) return false;
    IFormat* format = 0;
    switch (fType) {
    case ftFile:
        format = factory.createObject(path);
        break;
    case ftDirectory:
        format = new VCFDirectory();
        break;
    case ftAuto:
    case ftNew: // Only to avoid warning :(
        return false;
    }
    if (!format) {
        fatalError = factory.error;
        return false;
    }
    if (!checkForCSVProfile(format, items.originalProfile)) {
        fatalError = format->fatalError();
        delete format;
        return false;
    }
    bool res = format->setCharSet(gd.saveCharSet);
    if (!res && !gd.saveCharSet.isEmpty() && gd.saveCharSet!="UTF-8")
        errors << tr("This format not support charset in settings");
    res = format->exportRecords(path, items);
    if (res) {
        // Add _old_ name to recent
        if ((!_source.isEmpty()) && (_sourceType!=ftNew) && (path!=_source))
            _recent.addItem(_source);
        // Other updates
        _source = path;
        _sourceType = fType;
        _changed = false;
    }
    fatalError = format->fatalError();
    errors = format->errors();
    delete format;
    return res;
}

void ContactModel::close()
{
    _changed = false;
    beginResetModel();
    if (!_source.isEmpty() && _sourceType!=ftNew)
        _recent.addItem(_source);
    _source.clear();
    _sourceType = ftNew;
    items.clear();
    endResetModel();
}

void ContactModel::addRow(const ContactItem& c)
{
    beginInsertRows(QModelIndex(), 0, 0);
    items.push_back(c);
    endInsertRows();
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory();
#endif
}

ContactItem& ContactModel::beginEditRow(QModelIndex& index)
{
    int eRow = index.row();
    items[eRow].prevFullName = items[eRow].fullName;
    return items[eRow];
}

void ContactModel::endEditRow(QModelIndex& index)
{
    _changed = true;
    QStringList droppedFullNames;
    QString oldName = items[index.row()].prevFullName;
    QString newName = items[index.row()].fullName;
    if ((!oldName.isEmpty()) && newName.isEmpty())
        droppedFullNames << oldName;
#ifdef WITH_CALLS
    items.updateCallHistory(droppedFullNames);
#endif
    emit dataChanged(index, index.sibling(index.row(), columnCount()-1));
}

void ContactModel::copyRows(QModelIndexList& indices, ContactModel* target)
{
    foreach(QModelIndex index, indices)
        target->addRow(items[index.row()]);
}

void ContactModel::removeAnyRows(QModelIndexList& indices)
{
    std::sort(indices.begin(), indices.end());
    // foreach not usable here - reverse order needed
    beginRemoveRows (QModelIndex(), 0, indices.count()-1);
    QStringList droppedFullNames;
    for (int i=indices.count()-1; i>=0; i--) {
        droppedFullNames << items[indices[i].row()].fullName;
        items.removeAt(indices[i].row());
    }
    endRemoveRows();
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory(droppedFullNames);
#endif
}

void ContactModel::swapNames(const QModelIndexList& indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].swapNames();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::splitNames(const QModelIndexList &indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].splitNames();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::joinNames(const QModelIndexList &indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].joinNames();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::trimNames(const QModelIndexList &indices, int maxNames, int maxLen)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].trimNames(maxNames, maxLen);
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::dropSlashes(const QModelIndexList &indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].dropSlashes();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::generateFullNames(const QModelIndexList &indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].fullName = items[index.row()].formatNames();
        endEditRow(index);
    }
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory();
#endif
}

void ContactModel::parseFullName(const QModelIndexList& indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].parseFullName();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::dropFullNames(const QModelIndexList &indices)
{
    QStringList droppedFullNames;
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        droppedFullNames << items[index.row()].fullName;
        items[index.row()].fullName.clear();
        endEditRow(index);
    }
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory(droppedFullNames);
#endif
}

void ContactModel::reverseFullNames(const QModelIndexList &indices)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].reverseFullName();
        endEditRow(index);
    }
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory();
#endif
}

void ContactModel::formatPhones(const QModelIndexList &indices, const QString &templ)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        items[index.row()].formatPhones(templ);
        items[index.row()].calculateFields();
        endEditRow(index);
    }
    _changed = true;
}

void ContactModel::splitNumbers(const QModelIndexList &indices)
{
    foreach(QModelIndex index, indices) {
        ContactItem& item = items[index.row()];
        int newLines = item.phones.count()-1;
        if (newLines<1)
            continue;
        beginInsertRows(QModelIndex(), index.row(), index.row() + newLines);
        for (int i=1; i<item.phones.count(); i++) {
            ContactItem nc;
            nc.names = item.names;
            nc.fullName = item.fullName;
            QString tType = Phone::standardTypes.translate(item.phones[i].types[0]);
            if (nc.names.count()<3)
                nc.names.push_back(tType);
            else
                nc.names[2] += " " + tType;
            nc.phones.push_back(item.phones[i]);
            items.push_back(nc);
        }
        while (item.phones.count()>1)
            item.phones.removeLast();
        endInsertRows();
    }
    _changed = true;
}

void ContactModel::intlPhonePrefix(const QModelIndexList &indices, int countryRule)
{
    foreach(QModelIndex index, indices) {
        beginEditRow(index);
        if (items[index.row()].intlPhonePrefix(countryRule))
            items[index.row()].calculateFields();
        endEditRow(index);
    }
    _changed = true;
#ifdef WITH_CALLS
    items.updateCallHistory();
#endif
}

void ContactModel::addGroup(const QString &group)
{
    if (items.addGroup(group))
        _changed = true;
}

void ContactModel::renameGroup(const QString &oldName, const QString &newName)
{
    if (items.renameGroup(oldName, newName))
        _changed = true;
}

void ContactModel::removeGroup(const QString &group)
{
    if (items.removeGroup(group))
        _changed = true;
}

void ContactModel::mergeGroups(const QString &unitedGroup, const QString &mergedGroup)
{
    items.mergeGroups(unitedGroup, mergedGroup);
    _changed = true;
}

void ContactModel::splitGroup(const QString &existGroup, const QString &newGroup, const QList<int> &movedIndicesInGroup)
{
    items.splitGroup(existGroup, newGroup, movedIndicesInGroup);
    _changed = true;
}

void ContactModel::hardSort(ContactList::SortType sortType)
{
    items.sort(sortType);
    _changed = true;
}

void ContactModel::massTagRemove(const QStringList &tagNames)
{
    if (items.massTagRemove(tagNames))
        _changed = true;
}

void ContactModel::setViewMode(ContactModel::ContactViewMode mode, ContactModel *target)
{
    bool needRefresh =
      ((_viewMode==ContactModel::Standard || _viewMode==ContactModel::StandardWithQuickView) &&
      (mode==ContactModel::CompareMain || mode==ContactModel::CompareOpposite))
      ||
      ((_viewMode==ContactModel::CompareMain || _viewMode==ContactModel::CompareOpposite) &&
      (mode==ContactModel::Standard || mode==ContactModel::StandardWithQuickView));
    // TODO add here DupSearch varzzzz
    if (needRefresh)
        beginResetModel();
    _viewMode = mode;
    if (mode==ContactModel::CompareMain) {
        items.compareWith(target->itemList());
        target->setViewMode(ContactModel::CompareOpposite, 0);
    }
    else if ((mode==ContactModel::Standard
         || mode==ContactModel::StandardWithQuickView) && target)
        target->setViewMode(ContactModel::Standard, 0);
    if (needRefresh)
        endResetModel();
}

ContactModel::ContactViewMode ContactModel::viewMode()
{
    return _viewMode;
}

ContactList &ContactModel::itemList()
{
    return items;
}

void ContactModel::testList()
{
    ContactItem c;
    _source = "Test list";
    items.emptyGroups << QString::fromUtf8("Base");
    beginInsertRows(QModelIndex(), 0, 3);

    // Example with ASCII names and multy-type phone
    c.clear();
    c.fullName = QString::fromUtf8("John Doe");
    c.names << QString::fromUtf8("Doe") << QString::fromUtf8("John");
    c.phones << Phone("+1-213-555-1234", "work", "pref"); // example from  RFC 2426
    c.photo.pType = "URL";
    c.photo.url = "http://www.abc.com/pub/photos/jqpublic.gif";
    c.calculateFields();
    items.push_back(c);

    // Example with UTF8 (Russian) names and email
    c.clear();
    c.fullName = QString::fromUtf8("Александр Попов");
    c.names << QString::fromUtf8("Попов") << QString::fromUtf8("Александр");
    c.phones << Phone("+79101234567", "cell"); // example for any cellular operator
    c.emails << Email("alexpopov@spb.ru", "internet");
    c.ims << Messenger("apopov.jabber.ru", "xmpp");
    c.calculateFields();
    items.push_back(c);

    // Example with UTF8 (German) names
    c.clear();
    c.fullName = QString::fromUtf8("Hans Köster");
    c.names << QString::fromUtf8("Köster") << QString::fromUtf8("Hans");
    c.phones << Phone("233", "home");
    c.phones << Phone("322", "work");
    c.groups << QString::fromUtf8("Authors");
    c.calculateFields();
    items.push_back(c);

    // Example with 4 names and without phones
    c.clear();
    c.fullName = QString::fromUtf8("Ernst Theodor Amadeus Hoffmann");
    c.names << "Hoffmann" << "Ernst" << "Theodor Amadeus";
    c.groups << QString::fromUtf8("Authors");
    c.groups << QString::fromUtf8("Primary");
    c.calculateFields();
    items.push_back(c);

    // SIM-card-specific example with surname only;
    c.clear();
    c.names.push_back(QString::fromUtf8("SIM Name"));
    c.phones << Phone("+78122128506", "work"); // Long live rock'n'roll!
    c.calculateFields();
    items.push_back(c);

    // Empty contact
/*    c.clear();
    c.calculateFields();
    items.push_back(c); */

    endInsertRows();
}

bool ContactModel::checkForCSVProfile(IFormat *format, const QString& originalProfile)
{
    CSVFile* cFormat = dynamic_cast<CSVFile*>(format);
    if (!cFormat)
        return true;
    if (originalProfile.isEmpty()) {
        emit requestCSVProfile(cFormat); // and wait for answer immediately
        return (!(cFormat->profile().isEmpty()));
    }
    else {
        cFormat->setProfile(originalProfile);
        return true;
    }
}
