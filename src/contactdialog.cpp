/* Double Contact
 *
 * Module: Contact adding/editing dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QVBoxLayout>

#include "contactdialog.h"
#include "ui_contactdialog.h"

#include "contactlist.h"
#include "datedetailsdialog.h"
#include "globals.h"
#include "phonetypedialog.h"

#define MIN_VISIBLE_NAMES 2
#define MIN_VISIBLE_TRIPLETS 1

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog),
    nameCount(0), phoneCount(0), emailCount(0), anniversaryCount(0)
{
    ui->setupUi(this);
    fillPhoneTypes(ui->cbPhoneType1);
    fillEmailTypes(ui->cbEmailType1);
    connect(ui->cbPhoneType1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(itemTypeChanged(const QString&)));
    connect(ui->btnDelPhone1, SIGNAL(clicked()), this, SLOT(slotDelTriplet()));
    connect(ui->cbEmailType1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(itemTypeChanged(const QString&)));
    connect(ui->btnDelEmail1, SIGNAL(clicked()), this, SLOT(slotDelTriplet()));
    layAnniversaries = new QGridLayout(ui->gbAnniversaries);
    // Photo editing
    menuPhotoEdit = new QMenu(this);
    ui->btnPhotoEdit->setMenu(menuPhotoEdit);
    // Other known and unknown tags tables
    ui->twOtherTags->setItemDelegate(new ReadOnlyTableDelegate());
    ui->twUnknownTags->setItemDelegate(new ReadOnlyTableDelegate());
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ContactDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    ui->twOtherTags->setColumnWidth(0, ui->twOtherTags->width()*0.4);
    ui->twOtherTags->setColumnWidth(1, ui->twOtherTags->width()*0.5);
    ui->twUnknownTags->setColumnWidth(0, ui->twUnknownTags->width()*0.4);
    ui->twUnknownTags->setColumnWidth(1, ui->twUnknownTags->width()*0.5);
}

void ContactDialog::showEvent(QShowEvent*)
{
    resizeEvent(0);
}

void ContactDialog::clearData()
{
    setWindowTitle(tr("Add contact"));
    nameCount = MIN_VISIBLE_NAMES;
    phoneCount = emailCount = MIN_VISIBLE_TRIPLETS;
    ui->cbBirthday->setChecked(false);
    ui->dteBirthday->setEnabled(false);
}

void ContactDialog::setData(const ContactItem& c)
{
    setWindowTitle(tr("Edit contact"));
    // Names
    ui->leFullName->setText(c.fullName);
    for (int i=0; i<c.names.count(); i++)
            addName(c.names[i]);
    // Phones
    for (int i=0; i<c.phones.count(); i++)
        addPhone(c.phones[i]);
    // Emails
    for (int i=0; i<c.emails.count(); i++)
        addEmail(c.emails[i]);
    // Birthday and anniversaries
    ui->cbBirthday->setChecked(c.birthday.value.isValid());
    ui->dteBirthday->setEnabled(c.birthday.value.isValid());
    ui->btnBDayDetails->setEnabled(c.birthday.value.isValid());
    if (c.birthday.value.isValid())
        ui->dteBirthday->setDateTime(c.birthday.value);
    birthdayDetails = c.birthday;
    DateDetailsDialog::setDateFormat(ui->dteBirthday, birthdayDetails.hasTime);
    anniversaryDetails.clear();
    for (int i=0; i<c.anniversaries.count(); i++)
        addAnniversary(c.anniversaries[i]);
    // Photo
    if (c.photoType=="URL")
        ui->lbPhotoContent->setText(c.photoUrl);
    else if (c.photoType.toUpper()=="JPEG" || c.photoType.toUpper()=="PNG") {
        photo = c.photo;
        QPixmap pixPhoto;
        pixPhoto.loadFromData(photo);
        ui->lbPhotoContent->setPixmap(pixPhoto);
    }
    else if (!c.photo.isEmpty())
        ui->lbPhotoContent->setText(S_PH_UNKNOWN_FORMAT);
    updatePhotoMenu();
    // Addresses
    addAddress(ui->gbAddrHome, c.addrHome);
    addAddress(ui->gbAddrWork, c.addrWork);
    //Internet
    ui->leNickName->setText(c.nickName);
    ui->leURL->setText(c.url);
    ui->leJabber->setText(c.jabberName);
    ui->leICQ->setText(c.icqName);
    ui->leSkype->setText(c.skypeName);
    // Work
    ui->leOrganization->setText(c.organization);
    ui->leTitle->setText(c.title);
    // Other
    ui->leSortString->setText(c.sortString);
    ui->edDescription->setPlainText(c.description);
    ui->lbOriginalFormatValue->setText(c.originalFormat);
    ui->lbFormatVersionValue->setText(c.version);
    ui->lbIDValue->setText(c.id);
    ui->twOtherTags->setRowCount(c.otherTags.count());
    int index = 0;
    foreach (const TagValue& tag, c.otherTags) {
        ui->twOtherTags->setItem(index, 0, new QTableWidgetItem(tag.tag));
        ui->twOtherTags->setItem(index, 1, new QTableWidgetItem(tag.value));
        index++;
    }
    // Unknown tags
    ui->twUnknownTags->setRowCount(c.unknownTags.count());
    index = 0;
    foreach (const TagValue& tag, c.unknownTags) {
        ui->twUnknownTags->setItem(index, 0, new QTableWidgetItem(tag.tag));
        ui->twUnknownTags->setItem(index, 1, new QTableWidgetItem(tag.value));
        index++;
    }
}

void ContactDialog::getData(ContactItem& c)
{
    // Names
    c.fullName = ui->leFullName->text();
    c.names.clear();
    for (int i=0; i<nameCount; i++)
        c.names.push_back(findChild<QLineEdit*>(QString("leName%1").arg(i+1))->text());
    // Phones
    c.phones.clear();
    for (int i=0; i<phoneCount; i++) {
        Phone ph;
        readTriplet("Phone", i+1, ph.number, ph.tTypes, Phone::standardTypes);
        c.phones.push_back(ph);
    }
    // Emails
    c.emails.clear();
    for (int i=0; i<emailCount; i++) {
        Email em;
        readTriplet("Email", i+1, em.address, em.emTypes, Email::standardTypes);
        c.emails.push_back(em);
    }
    // Birthday and anniversaries
    c.birthday = birthdayDetails;
    c.birthday.value = (ui->cbBirthday->isChecked()) ? ui->dteBirthday->dateTime() : QDateTime();
    for (int i=0; i<anniversaryCount; i++) {
        DateItem di;
        readAnniversary(i+1, di);
        c.anniversaries.push_back(di);
    }
    // Photo
    QString photoText = ui->lbPhotoContent->text();
    if ((!photoText.isEmpty()) && photoText!=S_PH_UNKNOWN_FORMAT) { // URL
        c.photoType = "URL";
        c.photoUrl = photoText;
    }
    else if (!photo.isEmpty()) { // image
        c.photoType = detectPhotoFormat();
        c.photo = photo;
    }
    else { // deleted by user
        c.photoType.clear();
        c.photoUrl.clear();
        c.photo.clear();
    }
    // Addresses
    readAddress(ui->gbAddrHome, c.addrHome);
    if (!c.addrHome.isEmpty() && !c.addrHome.paTypes.contains("home", Qt::CaseInsensitive))
        c.addrHome.paTypes << "home";
    readAddress(ui->gbAddrWork, c.addrWork);
    if (!c.addrWork.isEmpty() && !c.addrWork.paTypes.contains("work", Qt::CaseInsensitive))
        c.addrWork.paTypes << "work";
    // Internet
    c.nickName = ui->leNickName->text();
    c.url = ui->leURL->text();
    c.jabberName = ui->leJabber->text();
    c.icqName = ui->leICQ->text();
    c.skypeName = ui->leSkype->text();
    // Work
    c.organization = ui->leOrganization->text();
    c.title = ui->leTitle->text();
    // Other
    c.sortString = ui->leSortString->text();
    c.description = ui->edDescription->toPlainText();
    c.calculateFields();
}

void ContactDialog::fillPhoneTypes(QComboBox* combo)
{
    combo->clear();
    combo->insertItems(0, Phone::standardTypes.displayValues);
    combo->addItem(S_MIXED_TYPE);
}

void ContactDialog::fillEmailTypes(QComboBox* combo)
{
    combo->clear();
    combo->insertItems(0, Email::standardTypes.displayValues);
    combo->addItem(S_MIXED_TYPE);
}

void ContactDialog::addName(const QString& name)
{
    if (nameCount>=MIN_VISIBLE_NAMES) {
        QLineEdit* le = new QLineEdit(this);
        le->setObjectName(QString("leName%1").arg(nameCount+1));
        ui->layNames->addWidget(le, nameCount-1, 0);
        // Delete button
        QToolButton* btnD = addDelButton(nameCount, "Name", SLOT(slotDelName()));
        ui->layNames->addWidget(btnD, nameCount-1, 1);
    }
    nameEditorByNum(nameCount+1)->setText(name);
    nameCount++;
}

void ContactDialog::slotDelName()
{
    QString oName = sender()->objectName();
    oName.remove("btnDelName");
    int oNumber = oName.toInt();
    for (int i=oNumber; i<nameCount; i++)
        nameEditorByNum(i)->setText(nameEditorByNum(i+1)->text());
    delete nameEditorByNum(nameCount);
    QToolButton* btnD = findChild<QToolButton*>(QString("btnDelName%1").arg(nameCount));
    delete btnD;
    nameCount--;
}

void ContactDialog::addPhone(const Phone& ph)
{
    addTriplet(phoneCount, ui->layPhones, "Phone", ph.number);
    QComboBox* cbT = findChild<QComboBox*>(QString("cbPhoneType%1").arg(phoneCount));
    if (phoneCount>MIN_VISIBLE_TRIPLETS)
        fillPhoneTypes(cbT);
    addTypeList(phoneCount, "Phone", ph.tTypes, Phone::standardTypes);
}

void ContactDialog::addEmail(const Email &em)
{
    addTriplet(emailCount, ui->layEmails, "Email", em.address);
    QComboBox* cbT = findChild<QComboBox*>(QString("cbEmailType%1").arg(emailCount));
    if (emailCount>MIN_VISIBLE_TRIPLETS)
        fillEmailTypes(cbT);
    addTypeList(emailCount, "Email", em.emTypes, Email::standardTypes);
}

void ContactDialog::addAnniversary(const DateItem &ann)
{
    anniversaryDetails.push_back(ann);
    // Value
    QDateTimeEdit* dte = new QDateTimeEdit(this);
    dte->setObjectName(QString("dteAnn%1Date").arg(anniversaryCount+1));
    dte->setDateTime(ann.value);
    DateDetailsDialog::setDateFormat(dte, ann.hasTime);
    dte->setCalendarPopup(true);
    layAnniversaries->addWidget(dte, anniversaryCount, 0);
    // Details button
    QPushButton* btnDet = new QPushButton(this);
    btnDet->setObjectName(QString("btnAnn%1Details").arg(anniversaryCount+1));
    btnDet->setText(ui->btnBDayDetails->text());
    connect(btnDet, SIGNAL(clicked()), this, SLOT(slotAnnDetails()));
    layAnniversaries->addWidget(btnDet, anniversaryCount, 1);
    // Delete button
    QToolButton* btnD = addDelButton(anniversaryCount, "Ann", SLOT(slotDelAnniversary()));
    layAnniversaries->addWidget(btnD, anniversaryCount, 2);
    anniversaryCount++;
}

void ContactDialog::readAnniversary(int num, DateItem &ann)
{
    QDateTimeEdit* editor = findChild<QDateTimeEdit*>
        (QString("dteAnn%1Date").arg(num));
    if (!editor) return;
    anniversaryDetails[num-1].value = editor->dateTime();
    ann = anniversaryDetails[num-1];
}

void ContactDialog::addAddress(QWidget *parent, const PostalAddress &addr)
{
    QGridLayout* l = new QGridLayout(parent);
    QLineEdit* le;
    // Address components
    l->addWidget(new QLabel(S_ADR_OFFICE_BOX), 0, 0);
    le = new QLineEdit(addr.offBox);
    le->setObjectName(S_ADR_OFFICE_BOX);
    l->addWidget(le, 0, 1);
    l->addWidget(new QLabel(S_ADR_EXTENDED), 1, 0);
    le = new QLineEdit(addr.extended);
    le->setObjectName(S_ADR_EXTENDED);
    l->addWidget(le, 1, 1);
    l->addWidget(new QLabel(S_ADR_STREET), 2, 0);
    le = new QLineEdit(addr.street);
    le->setObjectName(S_ADR_STREET);
    l->addWidget(le, 2, 1);
    l->addWidget(new QLabel(S_ADR_CITY), 3, 0);
    le = new QLineEdit(addr.city);
    le->setObjectName(S_ADR_CITY);
    l->addWidget(le, 3, 1);
    l->addWidget(new QLabel(S_ADR_REGION), 4, 0);
    le = new QLineEdit(addr.region);
    le->setObjectName(S_ADR_REGION);
    l->addWidget(le, 4, 1);
    l->addWidget(new QLabel(S_ADR_POST_CODE), 5, 0);
    le = new QLineEdit(addr.postalCode);
    le->setObjectName(S_ADR_POST_CODE);
    l->addWidget(le, 5, 1);
    l->addWidget(new QLabel(S_ADR_POST_CODE), 5, 0);
    le = new QLineEdit(addr.postalCode);
    le->setObjectName(S_ADR_POST_CODE);
    l->addWidget(le, 5, 1);
    l->addWidget(new QLabel(S_ADR_COUNTRY), 6, 0);
    le = new QLineEdit(addr.country);
    le->setObjectName(S_ADR_COUNTRY);
    l->addWidget(le, 6, 1);
    // TODO type box, if multy addresses will implemented
}

void ContactDialog::setAddress(QWidget *parent, const PostalAddress &addr)
{
    QLineEdit* le;
    le = parent->findChild<QLineEdit*>(S_ADR_OFFICE_BOX);
    if (le) le->setText(addr.offBox);
    le = parent->findChild<QLineEdit*>(S_ADR_EXTENDED);
    if (le) le->setText(addr.extended);
    le = parent->findChild<QLineEdit*>(S_ADR_STREET);
    if (le) le->setText(addr.street);
    le = parent->findChild<QLineEdit*>(S_ADR_CITY);
    if (le) le->setText(addr.city);
    le = parent->findChild<QLineEdit*>(S_ADR_REGION);
    if (le) le->setText(addr.region);
    le = parent->findChild<QLineEdit*>(S_ADR_POST_CODE);
    if (le) le->setText(addr.postalCode);
    le = parent->findChild<QLineEdit*>(S_ADR_COUNTRY);
    if (le) le->setText(addr.country);
}

void ContactDialog::readAddress(QWidget *parent, PostalAddress &addr)
{
    QLineEdit* le;
    le = parent->findChild<QLineEdit*>(S_ADR_OFFICE_BOX);
    if (le) addr.offBox = le->text();
    le = parent->findChild<QLineEdit*>(S_ADR_EXTENDED);
    if (le) addr.extended = le->text();
    le = parent->findChild<QLineEdit*>(S_ADR_STREET);
    if (le) addr.street = le->text();
    le = parent->findChild<QLineEdit*>(S_ADR_CITY);
    if (le) addr.city = le->text();
    le = parent->findChild<QLineEdit*>(S_ADR_REGION);
    if (le) addr.region = le->text();
    le = parent->findChild<QLineEdit*>(S_ADR_POST_CODE);
    if (le) addr.postalCode = le->text();
    le = parent->findChild<QLineEdit*>(S_ADR_COUNTRY);
    if (le) addr.country = le->text();
}

void ContactDialog::addTriplet(int& count, QGridLayout* l, const QString& nameTemplate, const QString& itemValue)
{
    if (count>=MIN_VISIBLE_TRIPLETS) {
        // Value
        QLineEdit* le = new QLineEdit(this);
        le->setObjectName(QString("le%1%2").arg(nameTemplate).arg(count+1));
        l->addWidget(le, count, 0);
        // Item type combo box
        QComboBox* cbT = new QComboBox(this);
        cbT->setObjectName(QString("cb%1Type%2").arg(nameTemplate).arg(count+1));
        cbT->setMaxVisibleItems(32); // "mixed" (last item) must be strongly visible
        connect(cbT, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(itemTypeChanged(const QString&)));
        l->addWidget(cbT, count, 1);
        // Delete button
        QToolButton* btnD = addDelButton(count, nameTemplate, SLOT(slotDelTriplet()));
        l->addWidget(btnD, count, 2);
    }
    if (!itemValue.isEmpty()) // Don't delete first text in interactive mode
        editorByNum(nameTemplate, count+1)->setText(itemValue);
    count++;
}

void ContactDialog::readTriplet(const QString &nameTemplate, int num, QString &itemValue, QStringList &types, const ::StandardTypes& sTypes)
{
    QLineEdit* editor = findChild<QLineEdit*>(QString("le%1%2").arg(nameTemplate).arg(num));
    QComboBox* typeBox = findChild<QComboBox*>(QString("cb%1Type%2").arg(nameTemplate).arg(num));
    if (!editor || !typeBox) return;
    itemValue = editor->text();
    QString t = typeBox->currentText();
    QStringList tl = t.split("+");
    types.clear();
    foreach(const QString& te, tl)
        types.push_back(sTypes.unTranslate(te));
    // TODO maybe store non-standart type in common list if operator changed it to standard?
}

void ContactDialog::slotDelTriplet()
{
    QString oName = sender()->objectName();
    oName.remove("btnDelPhone");
    oName.remove("btnDelEmail");
    int oNumber = oName.toInt();
    if (sender()->objectName().contains("Phone"))
        delTriplet(phoneCount, "Phone", oNumber);
    else
        delTriplet(emailCount, "Email", oNumber);
}

void ContactDialog::delTriplet(int& count, const QString& nameTemplate, int num)
{
    QLineEdit* editor = findChild<QLineEdit*>(QString("le%1%2").arg(nameTemplate).arg(num));
    QComboBox* typeBox = findChild<QComboBox*>(QString("cb%1Type%2").arg(nameTemplate).arg(num));
    QToolButton* delBtn = findChild<QToolButton*>(QString("btnDel%1%2").arg(nameTemplate).arg(num));
    if (!editor || !typeBox || !delBtn) return;
    if (count<=MIN_VISIBLE_TRIPLETS) { // only clear if first
        editor->clear();
        for (int i=typeBox->count()-1; i>=0; i--)
            if (typeBox->itemText(i).contains("+"))
                typeBox->removeItem(i);
        typeBox->setCurrentIndex(0);
    }
    else { // delete otherwise
        delete editor;
        delete typeBox;
        delete delBtn;
        count--;
        // Rename next dynamically created controls for crash preventing
        for (int i=num; i<=count; i++) {
            QLineEdit* editor = findChild<QLineEdit*>(QString("le%1%2").arg(nameTemplate).arg(i+1));
            QComboBox* typeBox = findChild<QComboBox*>(QString("cb%1Type%2").arg(nameTemplate).arg(i+1));
            QToolButton* delBtn = findChild<QToolButton*>(QString("btnDel%1%2").arg(nameTemplate).arg(i+1));
            editor->setObjectName(QString("le%1%2").arg(nameTemplate).arg(i));
            typeBox->setObjectName(QString("cb%1Type%2").arg(nameTemplate).arg(i));
            delBtn->setObjectName(QString("btnDel%1%2").arg(nameTemplate).arg(i));
        }
    }
}

QToolButton* ContactDialog::addDelButton
    (int count, const QString &nameTemplate, const char* method/*, QGridLayout* l, int pos*/)
{
    QToolButton* btnD = new QToolButton(this);
    btnD->setObjectName(QString("btnDel%1%2").arg(nameTemplate).arg(count+1));
    QPixmap icoDel(":/res/../img/16x16/del.png");
    btnD->setIcon(icoDel);
    connect(btnD, SIGNAL(clicked()), this, method);
    return btnD;
}

void ContactDialog::addTypeList(int count, const QString &nameTemplate,
          const QStringList &types, const ::StandardTypes &sTypes)
{
    QComboBox* cbT = findChild<QComboBox*>(QString("cb%1Type%2").arg(nameTemplate).arg(count));
    if (types.isEmpty())
        return;
    // Select item or add mixed
    QString translated = "";
    if (types.count()>1) { // Multi types
        foreach (const QString& t, types)
            translated += sTypes.translate(t) + "+";
        translated.remove(translated.length()-1, 1);
        cbT->insertItem(0, translated);
        cbT->setCurrentIndex(0);
    }
    else { // One type
        bool isStandard;
        translated = sTypes.translate(types[0], &isStandard);
        if (!isStandard)
            cbT->addItem(translated);
        cbT->setCurrentIndex(cbT->findText(translated));
    }
}

QLineEdit* ContactDialog::nameEditorByNum(int num)
{
    return findChild<QLineEdit*>(QString("leName%1").arg(num));
}

QLineEdit* ContactDialog::editorByNum(const QString& nameTemplate, int num)
{
    return findChild<QLineEdit*>(QString("le%1%2").arg(nameTemplate).arg(num));
}

void ContactDialog::editDateDetails(QDateTimeEdit *editor, DateItem &details)
{
    details.value = editor->dateTime();
    DateDetailsDialog* dlg = new DateDetailsDialog();
    dlg->setData(details);
    dlg->exec();
    if (dlg->result()==QDialog::Accepted) {
        dlg->getData(details);
        DateDetailsDialog::setDateFormat(editor, details.hasTime);
        editor->setDateTime(details.value);
    }
    delete dlg;
}

void ContactDialog::updatePhotoMenu()
{
    menuPhotoEdit->clear();
    menuPhotoEdit->addAction(S_PH_LOAD_IMAGE, this, SLOT(onLoadImage()));
    if (!photo.isEmpty())
        menuPhotoEdit->addAction(S_PH_SAVE_IMAGE, this, SLOT(onSaveImage()));
    menuPhotoEdit->addAction(S_PH_SET_URL, this, SLOT(onSetPhotoUrl()));
    menuPhotoEdit->addAction(S_PH_REMOVE, this, SLOT(onRemovePhoto()));
}

QString ContactDialog::detectPhotoFormat()
{
    QString format = "UNKNOWN";
    if (photo.mid(6, 4).contains("JFIF"))
        format = "JPEG";
    else if (photo.mid(1, 3).contains("PNG"))
        format = "PNG";
    return format;
}

void ContactDialog::on_btnAdd_clicked()
{
    QString subj = ui->cbAdd->currentText();
    if (subj==tr("name"))
        addName("");
    else if (subj==tr("phone"))
        addPhone(Phone());
    else if (subj==tr("email"))
        addEmail(Email());
    else if (subj==tr("anniversary"))
        addAnniversary(DateItem());
}

void ContactDialog::itemTypeChanged(const QString &value)
{
    if (value==S_MIXED_TYPE) {
        QComboBox *cbT = dynamic_cast<QComboBox*>(sender());
        // make dialog for mixed record
        if (sender()->objectName().contains("Phone"))
            PhoneTypeDialog::selectType(tr("Phone type"), Phone::standardTypes, cbT);
        else if (sender()->objectName().contains("Email"))
            PhoneTypeDialog::selectType(tr("Email type"), Email::standardTypes, cbT);
    }
}

void ContactDialog::on_cbBirthday_toggled(bool checked)
{
    ui->dteBirthday->setEnabled(checked);
    ui->btnBDayDetails->setEnabled(checked);
}

void ContactDialog::on_btnBDayDetails_clicked()
{
    editDateDetails(ui->dteBirthday, birthdayDetails);
}

void ContactDialog::slotAnnDetails()
{
    QString oName = sender()->objectName();
    oName.remove("btnAnn");
    oName.remove("Details");
    int oNumber = oName.toInt();
    QDateTimeEdit* editor = findChild<QDateTimeEdit*>
        (QString("dteAnn%1Date").arg(oNumber));
    if (editor)
        editDateDetails(editor, anniversaryDetails[oNumber-1]);
}

void ContactDialog::slotDelAnniversary()
{
    QString oName = sender()->objectName();
    oName.remove("btnDelAnn");
    int oNumber = oName.toInt();
    QDateTimeEdit* editor = findChild<QDateTimeEdit*>
        (QString("dteAnn%1Date").arg(oNumber));
    QPushButton* detBtn = findChild<QPushButton*>
        (QString("btnAnn%1Details").arg(oNumber));
    QToolButton* delBtn = findChild<QToolButton*>
        (QString("btnDelAnn%1").arg(oNumber));
    if (!editor || !detBtn || !delBtn) return;
    delete editor;
    delete detBtn;
    delete delBtn;
    anniversaryCount--;
    for (int i=oNumber; i<=anniversaryCount; i++) {
        QDateTimeEdit* editor = findChild<QDateTimeEdit*>
            (QString("dteAnn%1Date").arg(i+1));
        QPushButton* detBtn = findChild<QPushButton*>
            (QString("btnAnn%1Details").arg(i+1));
        QToolButton* delBtn = findChild<QToolButton*>
            (QString("btnDelAnn%1").arg(i+1));
        editor->setObjectName(QString("dteAnn%1Date").arg(i));
        detBtn->setObjectName(QString("btnAnn%1Details").arg(i));
        delBtn->setObjectName(QString("btnDelAnn%1").arg(i));
    }
}

void ContactDialog::on_twContact_currentChanged(int)
{
    resizeEvent(0);
}

void ContactDialog::on_btnSwapAddresses_clicked()
{
    PostalAddress bH, bW;
    readAddress(ui->gbAddrHome, bH);
    readAddress(ui->gbAddrWork, bW);
    setAddress(ui->gbAddrHome, bW);
    setAddress(ui->gbAddrWork, bH);
}

void ContactDialog::onLoadImage()
{
    QString path = ""; // TODO use lastImageFile() after ConfigManager implementation
    path = QFileDialog::getOpenFileName(0, tr("Open image file"), path,
        S_ALL_SUPPORTED.arg("(*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG)") +
        ";;JPEG (*.jpg *.JPG *.jpeg *.JPEG)" +
        ";;PNG (*.png *.PNG)" +
        ";;" + S_ALL_FILES);
    if (path.isEmpty())
        return;
    // TODO use setLastImageFile() after ConfigManager implementation
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, S_ERROR, S_READ_ERR.arg(path));
        return;
    }
    onRemovePhoto();
    photo = f.readAll();
    f.close();
    QPixmap pixPhoto;
    pixPhoto.loadFromData(photo);
    ui->lbPhotoContent->setPixmap(pixPhoto);
    updatePhotoMenu();
}

void ContactDialog::onSaveImage()
{
    QString path = ""; // TODO use lastImageFile() after ConfigManager implementation
    // but only dir!!!
    QString format = detectPhotoFormat();
    QString filter = "Binary (*.bin)";
    if (format=="JPEG")
        filter = "JPEG (*.jpg)";
    else if (format=="PNG")
        filter = "PNG (*.png)";
    path = QFileDialog::getSaveFileName(0, tr("Save image file"), path, filter);
    if (path.isEmpty())
        return;
    // TODO use setLastImageFile() after ConfigManager implementation
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(0, S_ERROR, S_WRITE_ERR.arg(path));
        return;
    }
    f.write(photo);
    f.close();
}

void ContactDialog::onSetPhotoUrl()
{
    QDialog* d = new QDialog(0);
    d->setWindowTitle(S_PH_SET_URL);
    QVBoxLayout* l = new QVBoxLayout();
    d->setLayout(l);
    QLineEdit* leURL = new QLineEdit(ui->lbPhotoContent->text());
    l->addWidget(leURL);
    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), d, SLOT(reject()));
    l->addWidget(bb);
    d->exec();
    if (d->result()==QDialog::Accepted) {
        onRemovePhoto();
        ui->lbPhotoContent->setText(leURL->text());
    }
    delete d;
}

void ContactDialog::onRemovePhoto()
{
    photo.clear();
    ui->lbPhotoContent->clear(); // remove pixmap and text, if were
}

