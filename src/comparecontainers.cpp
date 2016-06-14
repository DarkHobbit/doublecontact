/* Double Contact
 *
 * Module: Helpers for contact comparison dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QPalette>
#include <QVBoxLayout>
#include "comparecontainers.h"

#include <QMessageBox> // TODO ==>

ItemPair::ItemPair(const QString& title, QGridLayout* layout, bool multiItem)
    :QObject(layout), _multiItem(multiItem)
{
    int oldRowCount = layout->rowCount();
    gbLeft = new QGroupBox(title);
    gbRight = new QGroupBox(title);
    layLeft = new QGridLayout(gbLeft);
    layRight = new QGridLayout(gbRight);
    btnToLeft = new QToolButton();
    btnToLeft->setArrowType(Qt::LeftArrow);
    btnToRight = new QToolButton();
    btnToRight->setArrowType(Qt::RightArrow);
    layout->addWidget(gbLeft, oldRowCount, 0);
    layout->addWidget(btnToLeft, oldRowCount, 1);
    layout->addWidget(btnToRight, oldRowCount, 2);
    layout->addWidget(gbRight, oldRowCount, 3);
    connect(btnToLeft, SIGNAL(clicked()), this, SLOT(onToLeftClicked()));
    connect(btnToRight, SIGNAL(clicked()), this, SLOT(onToRightClicked()));
}

void ItemPair::highlightDiff(bool hasDiff)
{
    QString sheet =
        QString("QGroupBox { border: 1px groove %1; border-radius: 2px; }")
        .arg(hasDiff ? "red" : "green");
    gbLeft->setStyleSheet(sheet);
    gbRight->setStyleSheet(sheet);
}

void ItemPair::buildOneItemButtons(int column)
{
    buildOneItemButtonSide(true, column);
    buildOneItemButtonSide(false, column);
}

void ItemPair::buildOneItemButtonSide(bool toLeft, int column)
{
    QList<QToolButton *> &btnSet = toLeft ? btnsOneItemToLeft : btnsOneItemToRight;
    QGridLayout *layout = toLeft ? layRight : layLeft;
    while (!btnSet.isEmpty()) {
        delete btnSet.last();
        btnSet.removeLast();
    }
    if (!layout->isEmpty())
    for (int i=0; i<layout->rowCount(); i++)
        addOneItemButton(toLeft, column);
}

void ItemPair::addOneItemButton(bool toLeft, int column)
{
    QList<QToolButton *> &btnSet = toLeft ? btnsOneItemToLeft : btnsOneItemToRight;
    QGridLayout *layout = toLeft ? layRight : layLeft;
    QToolButton* btn = new QToolButton();
    btnSet.push_back(btn);
    btn->setArrowType(toLeft ? Qt::LeftArrow : Qt::RightArrow);
    layout->addWidget(btn, btnSet.count()-1, column);
    connect(btn, SIGNAL(clicked()), this, toLeft ? SLOT(onOneItemToLeftClicked()) : SLOT(onOneItemToRightClicked()));
}

void ItemPair::onToLeftClicked()
{
    copyData(true);
    highlightDiff(false);
}

void ItemPair::onToRightClicked()
{
    copyData(false);
    highlightDiff(false);
}

void ItemPair::onOneItemToLeftClicked()
{
    copyOneItem(true, btnsOneItemToLeft.indexOf(dynamic_cast<QToolButton*>(sender())));
    highlightDiff(checkDiff());
}

void ItemPair::onOneItemToRightClicked()
{
    copyOneItem(false, btnsOneItemToRight.indexOf(dynamic_cast<QToolButton*>(sender())));
    highlightDiff(checkDiff());
}

StringListPair::StringListPair(const QString& title, QGridLayout *layout,
    const QStringList &leftData, const QStringList &rightData, bool multiItem)
    :ItemPair(title, layout, multiItem)
{
    fillBox(layLeft, leftData, leftSet);
    fillBox(layRight, rightData, rightSet);
    highlightDiff(leftData!=rightData);
    if (_multiItem)
        buildOneItemButtons(1);
}

void StringListPair::getData(QStringList &leftData, QStringList &rightData)
{
    leftData.clear();
    foreach(QLineEdit* ed, leftSet)
        leftData.push_back(ed->text());
    rightData.clear();
    foreach(QLineEdit* ed, rightSet)
        rightData.push_back(ed->text());
}

void StringListPair::copyData(bool toLeft)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftSet : rightSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightSet : leftSet;
    QGridLayout *layDest = toLeft ? layLeft : layRight;
    while (edSetDest.count()>edSetSrc.count())
    {
        delete edSetDest.last();
        edSetDest.removeLast();
    }
    while (edSetDest.count()<edSetSrc.count()) {
        edSetDest.push_back(new QLineEdit());
        layDest->addWidget(edSetDest.last());
        this->addOneItemButton(!toLeft, 1);
    }
    for (int i=0; i<edSetDest.count(); i++)
        edSetDest[i]->setText(edSetSrc[i]->text());
}

void StringListPair::copyOneItem(bool toLeft, int srcIndex)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftSet : rightSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightSet : leftSet;
    QGridLayout *layDest = toLeft ? layLeft : layRight;
    edSetDest.push_back(new QLineEdit());
    edSetDest.last()->setText(edSetSrc[srcIndex]->text());
    layDest->addWidget(edSetDest.last());
    this->addOneItemButton(!toLeft, 1);
}

void StringListPair::fillBox(QGridLayout* layout, const QStringList &data, QList<QLineEdit*>& edSet)
{
    foreach(const QString& s, data) {
        edSet.push_back(new QLineEdit(s));
        layout->addWidget(edSet.last());
    }
}

StringPair::StringPair(const QString &title, QGridLayout *layout,
    const QString &leftData, const QString &rightData)
    :StringListPair(title, layout, QStringList(leftData), QStringList(rightData), false)
{}

void StringPair::getData(QString &leftData, QString &rightData)
{
    QStringList ll, rl;
    StringListPair::getData(ll, rl);
    leftData = ll[0];
    rightData = rl[0];
}

TypedPair::TypedPair(const QString &title, QGridLayout *layout)
    :ItemPair(title, layout, true), standardTypes(0)
{}

TypedPair::~TypedPair()
{
    delete standardTypes;
}

void TypedPair::addValue(const QString &value, const QStringList &types, bool toLeft)
{
    QGridLayout* layout = toLeft ? layLeft : layRight;
    QList<QLineEdit*>& edSet = toLeft ? leftEdSet : rightEdSet;
    QList<QComboBox*>& comboSet = toLeft ? leftComboSet : rightComboSet;
    int prevCount = edSet.count();
    edSet.push_back(new QLineEdit(value));
    layout->addWidget(edSet.last(), prevCount, 0);
    QComboBox* combo = new QComboBox();
    comboSet.push_back(combo);
    // Add standard values
    foreach(const QString& sv, standardTypes->displayValues)
            combo->addItem(sv);
    // Add current value(s)
    if (types.count()==1) {
        bool isStandard;
        QString dType = standardTypes->translate(types[0], &isStandard);
        if (!isStandard)
            combo->addItem(dType);
        combo->setCurrentIndex(combo->findText(dType, Qt::MatchExactly));
    }
    else {
        // TODO compare for multi-type typed pairs
        QMessageBox::warning(0, S_WARNING,
        QString("Compare for multi-type typed pairs not implemented! Current version may corrupt data"));
        combo->addItem(types[0]); //==> spec. value instead
    }
    layout->addWidget(combo, prevCount, 1);
}

int TypedPair::count(bool onLeft)
{
    return (onLeft ? layLeft : layRight)->rowCount();
}

bool TypedPair::getValue(int index, QString &value, QStringList &types, bool fromLeft)
{
    QList<QLineEdit*>& edSet = fromLeft ? leftEdSet : rightEdSet;
    QList<QComboBox*>& comboSet = fromLeft ? leftComboSet : rightComboSet;
    if (index>=edSet.count()) return false;
    value = edSet[index]->text();
    types.clear();
    QString typeValue = comboSet[index]->currentText();
    if (false /*typeValue==???*/) {
        // TODO compare for multi-type typed pairs
    }
    else
        types << standardTypes->unTranslate(typeValue);
    return true;
}

void TypedPair::copyData(bool toLeft)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftEdSet : rightEdSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightEdSet : leftEdSet;
    QList<QComboBox*>& comboSetDest = toLeft ? leftComboSet : rightComboSet;
    QList<QComboBox*>& comboSetSrc = toLeft ? rightComboSet : leftComboSet;
    QGridLayout* layDest = toLeft ? layLeft : layRight;
    while (edSetDest.count()>edSetSrc.count())
    {
        delete edSetDest.last();
        edSetDest.removeLast();
        delete comboSetDest.last();
        comboSetDest.removeLast();
    }
    while (edSetDest.count()<edSetSrc.count()) {
        int prevCount = layDest->rowCount();
        edSetDest.push_back(new QLineEdit());
        layDest->addWidget(edSetDest.last(), prevCount, 0);
        comboSetDest.push_back(new QComboBox());
        layDest->addWidget(comboSetDest.last(), prevCount, 1);
        this->addOneItemButton(!toLeft, 2);
    }
    for (int i=0; i<edSetDest.count(); i++) {
        edSetDest[i]->setText(edSetSrc[i]->text());
        comboSetDest[i]->clear();
        for (int j=0; j<comboSetSrc[i]->count(); j++)
            comboSetDest[i]->addItem(comboSetSrc[i]->itemText(j));
        comboSetDest[i]->setCurrentIndex(comboSetSrc[i]->currentIndex());
    }
}

void TypedPair::copyOneItem(bool toLeft, int srcIndex)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftEdSet : rightEdSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightEdSet : leftEdSet;
    QList<QComboBox*>& comboSetDest = toLeft ? leftComboSet : rightComboSet;
    QList<QComboBox*>& comboSetSrc = toLeft ? rightComboSet : leftComboSet;
    QGridLayout* layDest = toLeft ? layLeft : layRight;
    int prevCount = layDest->rowCount();
    edSetDest.push_back(new QLineEdit());
    edSetDest.last()->setText(edSetSrc[srcIndex]->text());
    layDest->addWidget(edSetDest.last());
    comboSetDest.push_back(new QComboBox());
    layDest->addWidget(comboSetDest.last(), prevCount, 1);
    for (int i=0; i<comboSetSrc[srcIndex]->count(); i++)
        comboSetDest.last()->addItem(comboSetSrc[srcIndex]->itemText(i));
    comboSetDest.last()->setCurrentIndex(comboSetSrc[srcIndex]->currentIndex());
    this->addOneItemButton(!toLeft, 2);
}

PhonesPair::PhonesPair(const QString &title, QGridLayout *layout, const QList<Phone> &leftPhones, const QList<Phone> &rightPhones)
    :TypedPair(title, layout)
{
    standardTypes = new Phone::StandardTypes();
    foreach(const Phone& p, leftPhones)
        addValue(p.number, p.tTypes, true);
    foreach(const Phone& p, rightPhones)
        addValue(p.number, p.tTypes, false);
    highlightDiff(leftPhones!=rightPhones);
    buildOneItemButtons(2);
}

void PhonesPair::getData(QList<Phone> &leftPhones, QList<Phone> &rightPhones)
{
    leftPhones.clear();
    for (int i=0; i<count(true); i++) {
        Phone p;
        getValue(i, p.number, p.tTypes, true);
        leftPhones << p;
    }
    rightPhones.clear();
    for (int i=0; i<count(false); i++) {
        Phone p;
        getValue(i, p.number, p.tTypes, false);
        rightPhones << p;
    }
}

EmailsPair::EmailsPair(const QString &title, QGridLayout *layout, const QList<Email> &leftEmails, const QList<Email> &rightEmails)
    :TypedPair(title, layout)
{
    standardTypes = new Email::StandardTypes();
    foreach(const Email& p, leftEmails)
        addValue(p.address, p.emTypes, true);
    foreach(const Email& p, rightEmails)
        addValue(p.address, p.emTypes, false);
    highlightDiff(leftEmails!=rightEmails);
    buildOneItemButtons(2);
}

void EmailsPair::getData(QList<Email> &leftEmails, QList<Email> &rightEmails)
{
    leftEmails.clear();
    for (int i=0; i<count(true); i++) {
        Email p;
        getValue(i, p.address, p.emTypes, true);
        leftEmails << p;
    }
    rightEmails.clear();
    for (int i=0; i<count(false); i++) {
        Email p;
        getValue(i, p.address, p.emTypes, false);
        rightEmails << p;
    }
}


DateItemListPair::DateItemListPair(const QString &title, QGridLayout *layout,
    const QList<DateItem> &leftData, const QList<DateItem> &rightData, bool multiItem)
    :ItemPair(title, layout, multiItem)
{
    leftDataSet = leftData;
    rightDataSet = rightData;
    fillBox(layLeft, leftData, leftSet);
    fillBox(layRight, rightData, rightSet);
    highlightDiff(leftData!=rightData);
    if (_multiItem)
        buildOneItemButtons(1);
}

void DateItemListPair::getData(QList<DateItem> &leftData, QList<DateItem> &rightData)
{
    leftData = leftDataSet;
    rightData = rightDataSet;
}

void DateItemListPair::copyData(bool toLeft)
{
    QList<QLabel*>& lbSetDest = toLeft ? leftSet : rightSet;
    QList<DateItem>& dsDest = toLeft ?  leftDataSet : rightDataSet;
    QList<DateItem>& dsSrc= toLeft ?  rightDataSet : leftDataSet;
    QGridLayout* layDest = toLeft ? layLeft : layRight;
    dsDest = dsSrc;
    while (!lbSetDest.isEmpty())
    {
        delete lbSetDest.last();
        lbSetDest.removeLast();
    }
    fillBox(layDest, dsDest, lbSetDest);
    buildOneItemButtons(1);
}

void DateItemListPair::copyOneItem(bool toLeft, int srcIndex)
{
    QList<QLabel*>& lbSetDest = toLeft ? leftSet : rightSet;
    QList<QLabel*>& lbSetSrc = toLeft ? rightSet : leftSet;
    QGridLayout *layDest = toLeft ? layLeft : layRight;
    QList<DateItem>& dsDest = toLeft ? leftDataSet : rightDataSet;
    QList<DateItem>& dsSrc = toLeft ? rightDataSet : leftDataSet;
    lbSetDest.push_back(new QLabel());
    lbSetDest.last()->setText(lbSetSrc[srcIndex]->text());
    layDest->addWidget(lbSetDest.last());
    dsDest.push_back(dsSrc[srcIndex]);
    this->addOneItemButton(!toLeft, 1);
}

void DateItemListPair::fillBox(QGridLayout* layout, const QList<DateItem> &data, QList<QLabel *> &lbSet)
{
    foreach(const DateItem& di, data) {
        lbSet.push_back(new QLabel(di.toString()));
        layout->addWidget(lbSet.last());
    }
}


DateItemPair::DateItemPair(const QString &title, QGridLayout *layout,
    const DateItem &leftData, const DateItem &rightData)
    :DateItemListPair(title, layout, QList<DateItem>() << leftData, QList<DateItem>() << rightData, false)
{}

void DateItemPair::getData(DateItem &leftData, DateItem &rightData)
{
    QList<DateItem> ll, rl;
    DateItemListPair::getData(ll, rl);
    leftData = ll[0];
    rightData = rl[0];
}
