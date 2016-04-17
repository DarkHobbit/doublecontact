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

#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include "contactdialog.h"
#include "phonetypedialog.h"
#include "ui_contactdialog.h"

#include "contactlist.h"

// Spec.value for combined phone/mail types
const QString mixedType = QObject::tr("mixed...");
#define MIN_VISIBLE_NAMES 2
#define MIN_VISIBLE_TRIPLETS 1

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog),
    nameCount(0), phoneCount(0), emailCount(0)
{
    ui->setupUi(this);
    fillPhoneTypes(ui->cbPhoneType1);
    fillEmailTypes(ui->cbEmailType1);
    connect(ui->cbPhoneType1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(itemTypeChanged(const QString&)));
    connect(ui->btnDelPhone1, SIGNAL(clicked()), this, SLOT(slotDelTriplet()));
    connect(ui->cbEmailType1, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(itemTypeChanged(const QString&)));
    connect(ui->btnDelEmail1, SIGNAL(clicked()), this, SLOT(slotDelTriplet()));
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
    // BirthDay
    ui->cbBirthday->setChecked(c.birthday.value.isValid());
    ui->dteBirthday->setEnabled(c.birthday.value.isValid());
    if (c.birthday.value.isValid())
        ui->dteBirthday->setDateTime(c.birthday.value);
    // Other
    ui->edDescription->setPlainText(c.description);
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
    // BirthDay
    c.birthday.value = (ui->cbBirthday->isChecked()) ? ui->dteBirthday->dateTime() : QDateTime();
    // Other
    c.description = ui->edDescription->toPlainText();
    // TODO Photos, address
    c.calculateFields();
}

void ContactDialog::fillPhoneTypes(QComboBox* combo)
{
    combo->clear();
    combo->insertItems(0, Phone::standardTypes.displayValues);
    combo->addItem(mixedType);
}

void ContactDialog::fillEmailTypes(QComboBox* combo)
{
    combo->clear();
    combo->insertItems(0, Email::standardTypes.displayValues);
    combo->addItem(mixedType);
}

void ContactDialog::addName(const QString& name)
{
    if (nameCount>=MIN_VISIBLE_NAMES) {
        QLineEdit* le = new QLineEdit(this);
        le->setObjectName(QString("leName%1").arg(nameCount+1));
        ui->layNames->addWidget(le, nameCount-1, 0);
        QToolButton* btnD = new QToolButton(this);
        btnD->setObjectName(QString("btnDelName%1").arg(nameCount+1));
        QPixmap icoDel(":/res/../img/16x16/del.png");
        btnD->setIcon(icoDel);
        connect(btnD, SIGNAL(clicked()), this, SLOT(slotDelName()));
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
        QToolButton* btnD = new QToolButton(this);
        btnD->setObjectName(QString("btnDel%1%2").arg(nameTemplate).arg(count+1));
        QPixmap icoDel(":/res/../img/16x16/del.png");
        btnD->setIcon(icoDel);
        connect(btnD, SIGNAL(clicked()), this, SLOT(slotDelTriplet()));
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

void ContactDialog::on_btnAdd_clicked()
{
    if (ui->cbAdd->currentText()==tr("name"))
        addName("");
    else if (ui->cbAdd->currentText()==tr("phone"))
        addPhone(Phone());
    else
        addEmail(Email());
}

void ContactDialog::itemTypeChanged(const QString &value)
{
    if (value==mixedType) {
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
}
