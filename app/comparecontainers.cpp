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
#include <QPixmap>
#include <QVBoxLayout>

#include "comparecontainers.h"
#include "helpers.h"

#include <iostream>

// TODO make arrows up and down (to reorder items), not only left and right

ItemPair::ItemPair(const QString& title, QGridLayout* layout, bool multiItem)
    :QObject(layout), _multiItem(multiItem), _rowLimit(0)
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
    QString sheet = QString(
         // Original
         /*"QGroupBox { \
             border: 1px groove %1; \
             border-radius: 2px; \
             margin: 10px; \
             padding: 4px; \
         }"*/

         // UNION labs, http://www.prog.org.ru/index.php?topic=21738
         "QGroupBox { \
                border: 1px solid %1; \
                border-radius: 3px; \
                margin: 10px; \
                padding: 4px; \
             } \
             QGroupBox::title { \
                  subcontrol-origin: margin; \
                  subcontrol-position: top left; \
                padding: 4px; \
                left: 20px; \
              }"

           // spirits25, http://www.prog.org.ru/index.php?topic=21738
           /*"QGroupBox::title { \
                border: 0px outset %1; \
                border-radius: 6px; \
                background-color: rgba(255, 255, 255, 140); \
                subcontrol-origin: margin; \
                color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, \
                stop: 0 #111111, stop: 0.7 #111111, stop: 1 #888888); \
                subcontrol-position: top left; \
                padding: 1px 18px 1px 13px; \
            }"*/
    ).arg(hasDiff ? "red" : "green");
    gbLeft->setStyleSheet(sheet);
    gbRight->setStyleSheet(sheet);
}

void ItemPair::onItemChanged()
{
    highlightDiff(checkDiff());
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
    while (!btnSet.isEmpty())
        removeOneItemButton(toLeft);
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

void ItemPair::removeOneItemButton(bool toLeft)
{
    QList<QToolButton *> &btnSet = toLeft ? btnsOneItemToLeft : btnsOneItemToRight;
    delete btnSet.last();
    btnSet.removeLast();
}

QLineEdit *ItemPair::addEditor(const QString& text)
{
    QLineEdit* ed = new QLineEdit(text);
    connect(ed, SIGNAL(textChanged(const QString &)), this, SLOT(onItemChanged()));
    return ed;
}

QComboBox *ItemPair::addCombo()
{
    QComboBox* cb = new QComboBox();
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onItemChanged()));
    return cb;
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

StringListPair::StringListPair(const QString& title, QGridLayout *layout, bool multiItem)
    :ItemPair(title, layout, multiItem), hasLabels(false)
{}

void StringListPair::setData(const QStringList &leftData, const QStringList &rightData)
{
    fillBox(layLeft, leftData, leftSet, leftLabels);
    fillBox(layRight, rightData, rightSet, rightLabels);
    highlightDiff(leftData!=rightData);
    if (_multiItem)
        buildOneItemButtons(hasLabels ? 2 : 1);
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
    QList<QLabel*>& lbSetDest = toLeft ? leftLabels : rightLabels;
    QGridLayout *layDest = toLeft ? layLeft : layRight;
    while (edSetDest.count()>edSetSrc.count())
    {
        delete edSetDest.last();        
        edSetDest.removeLast();
        if (hasLabels) {
            delete lbSetDest.last();
            lbSetDest.removeLast();
        }
        removeOneItemButton(!toLeft);
    }
    while (edSetDest.count()<edSetSrc.count())
        addLine(edSetDest.count(), "", layDest, edSetDest, lbSetDest, true);
    for (int i=0; i<edSetDest.count(); i++)
        edSetDest[i]->setText(edSetSrc[i]->text());
}

void StringListPair::copyOneItem(bool toLeft, int srcIndex)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftSet : rightSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightSet : leftSet;
    QList<QLabel*>& lbSetDest = toLeft ? leftLabels : rightLabels;
    QGridLayout *layDest = toLeft ? layLeft : layRight;
    addLine(edSetDest.count(), edSetSrc[srcIndex]->text(), layDest, edSetDest, lbSetDest, true);
}

bool StringListPair::checkDiff()
{
    QStringList left, right;
    getData(left, right);
    return left!=right;
}

QString StringListPair::label(int)
{
    return "";
}

void StringListPair::fillBox(QGridLayout* layout, const QStringList &data, QList<QLineEdit*>& edSet, QList<QLabel*>& lbSet)
{
    for(int i=0; i<data.count(); i++)
        addLine(i, data[i], layout, edSet, lbSet, false);
}

// TODO maybe, drop ItemPair::buildOneItemB* method and port addOneItemButton to addLine in all classes
// After this, createOneItemButton parameter will not needed
void StringListPair::addLine(int row, const QString &text,
    QGridLayout* layout, QList<QLineEdit*>& edSet, QList<QLabel*>& lbSet,
    bool createOneItemButton)
{
    if (_rowLimit>0 && edSet.count()>=_rowLimit)
        return;
    if (hasLabels) {
        lbSet.push_back(new QLabel(label(row)));
        layout->addWidget(lbSet.last(), row, 0);
    }
    edSet.push_back(addEditor(text));
    layout->addWidget(edSet.last(), row, hasLabels ? 1 : 0);
    if (createOneItemButton && _multiItem)
        this->addOneItemButton(edSet==rightSet, hasLabels ? 2 : 1);
}

StringPair::StringPair(const QString &title, QGridLayout *layout)
    :StringListPair(title, layout, false)
{}

void StringPair::setData(const QString &leftData, const QString &rightData)
{
    StringListPair::setData(QStringList(leftData), QStringList(rightData));
}

void StringPair::getData(QString &leftData, QString &rightData)
{
    QStringList ll, rl;
    StringListPair::getData(ll, rl);
    leftData = ll[0];
    rightData = rl[0];
}

NamePair::NamePair(const QString &title, QGridLayout *layout)
    :StringListPair(title, layout, true)
{
    hasLabels = true;
    _rowLimit = MAX_NAMES;
}

QString NamePair::label(int row)
{
    return ContactItem::nameComponent(row);
}

TypedPair::TypedPair(const QString &title, QGridLayout *layout)
    :ItemPair(title, layout, true), standardTypes(0)
{}

TypedPair::~TypedPair()
{
}

template<class T>
void TypedPair::getData(QList<T> &leftItems, QList<T> &rightItems)
{
    leftItems.clear();
    for (int i=0; i<count(true); i++) {
        T p;
        getValue(i, p, true);
        leftItems << p;
    }
    rightItems.clear();
    for (int i=0; i<count(false); i++) {
        T p;
        getValue(i, p, false);
        rightItems << p;
    }
}

void TypedPair::addValue(const TypedStringItem& item, bool toLeft)
{
    QGridLayout* layout = toLeft ? layLeft : layRight;
    QList<QLineEdit*>& edSet = toLeft ? leftEdSet : rightEdSet;
    QList<QComboBox*>& comboSet = toLeft ? leftComboSet : rightComboSet;
    int prevCount = edSet.count();
    edSet.push_back(addEditor(item.value));
    layout->addWidget(edSet.last(), prevCount, 0);
    QComboBox* combo = addCombo();
    comboSet.push_back(combo);
    // Add standard values
    foreach(const QString& sv, standardTypes->displayValues)
            combo->addItem(sv);
    // Add current value(s)
    QString dType;
    if (item.types.count()==1) {
        bool isStandard;
        dType = standardTypes->translate(item.types[0], &isStandard);
        if (!isStandard)
            combo->addItem(dType);
    }
    else {
        foreach (const QString& ut, item.types) {
            if (!dType.isEmpty())
                dType += "+";
            dType += standardTypes->translate(ut);
        }
        combo->addItem(dType);
    }
    combo->setCurrentIndex(combo->findText(dType, Qt::MatchExactly));
    layout->addWidget(combo, prevCount, 1);
}

int TypedPair::count(bool onLeft)
{
    return (onLeft ? layLeft : layRight)->rowCount();
}

bool TypedPair::getValue(int index, TypedStringItem& item, bool fromLeft)
{
    QList<QLineEdit*>& edSet = fromLeft ? leftEdSet : rightEdSet;
    QList<QComboBox*>& comboSet = fromLeft ? leftComboSet : rightComboSet;
    if (index>=edSet.count()) return false;
    item.value = edSet[index]->text();
    item.types.clear();
    QString typeValue = comboSet[index]->currentText();
    if (typeValue.contains("+")) {
        QStringList tl = typeValue.split("+");
        item.types.clear();
        foreach(const QString& te, tl)
            item.types << standardTypes->unTranslate(te);
    }
    else
        item.types << standardTypes->unTranslate(typeValue);
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
        removeOneItemButton(!toLeft);
    }
    while (edSetDest.count()<edSetSrc.count()) {
        int prevCount = layDest->rowCount();
        edSetDest.push_back(addEditor(""));
        layDest->addWidget(edSetDest.last(), prevCount, 0);
        comboSetDest.push_back(addCombo());
        layDest->addWidget(comboSetDest.last(), prevCount, 1);
        if (_multiItem)
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
    edSetDest.push_back(addEditor(""));
    edSetDest.last()->setText(edSetSrc[srcIndex]->text());
    layDest->addWidget(edSetDest.last());
    comboSetDest.push_back(addCombo());
    layDest->addWidget(comboSetDest.last(), prevCount, 1);
    for (int i=0; i<comboSetSrc[srcIndex]->count(); i++)
        comboSetDest.last()->addItem(comboSetSrc[srcIndex]->itemText(i));
    comboSetDest.last()->setCurrentIndex(comboSetSrc[srcIndex]->currentIndex());
    this->addOneItemButton(!toLeft, 2);
}

PhonesPair::PhonesPair(const QString &title, QGridLayout *layout)
    :TypedPair(title, layout)
{
    standardTypes = &Phone::standardTypes;
}

void PhonesPair::setData(const QList<Phone> &leftPhones, const QList<Phone> &rightPhones)
{
    foreach(const Phone& p, leftPhones)
        addValue(p, true);
    foreach(const Phone& p, rightPhones)
        addValue(p, false);
    highlightDiff(leftPhones!=rightPhones);
    buildOneItemButtons(2);
}

bool PhonesPair::checkDiff()
{
    QList<Phone> leftPhones, rightPhones;
    getData(leftPhones, rightPhones);
    return leftPhones!=rightPhones;
}

EmailsPair::EmailsPair(const QString &title, QGridLayout *layout)
    :TypedPair(title, layout)
{
    standardTypes = &Email::standardTypes;
}

void EmailsPair::setData(const QList<Email> &leftEmails, const QList<Email> &rightEmails)
{
    foreach(const Email& p, leftEmails)
        addValue(p, true);
    foreach(const Email& p, rightEmails)
        addValue(p, false);
    highlightDiff(leftEmails!=rightEmails);
    buildOneItemButtons(2);
}

bool EmailsPair::checkDiff()
{
    QList<Email> leftEmails, rightEmails;
    getData(leftEmails, rightEmails);
    return leftEmails!=rightEmails;
}

DateItemListPair::DateItemListPair(const QString &title, QGridLayout *layout, bool multiItem)
    :ItemPair(title, layout, multiItem)
{}

void DateItemListPair::setData(const QList<DateItem> &leftData, const QList<DateItem> &rightData)
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

bool DateItemListPair::checkDiff()
{
    QList<DateItem> left, right;
    getData(left, right);
    return left!=right;

}

void DateItemListPair::fillBox(QGridLayout* layout, const QList<DateItem> &data, QList<QLabel *> &lbSet)
{
    foreach(const DateItem& di, data) {
        lbSet.push_back(new QLabel(di.toString(DateItem::Local)));
        layout->addWidget(lbSet.last());
    }
}


DateItemPair::DateItemPair(const QString &title, QGridLayout *layout)
    :DateItemListPair(title, layout, false)
{}

void DateItemPair::setData(const DateItem &leftData, const DateItem &rightData)
{
    DateItemListPair::setData(QList<DateItem>() << leftData, QList<DateItem>() << rightData);
}

void DateItemPair::getData(DateItem &leftData, DateItem &rightData)
{
    QList<DateItem> ll, rl;
    DateItemListPair::getData(ll, rl);
    leftData = ll[0];
    rightData = rl[0];
}

PostalAddressPair::PostalAddressPair(const QString &title, QGridLayout *layout)
    :TypedPair(title, layout)
{
    standardTypes = &PostalAddress::standardTypes;
}

void PostalAddressPair::setData(const QList<PostalAddress> &leftData, const QList<PostalAddress> &rightData)
{
    foreach (const PostalAddress& addr, leftData) {
        TypedStringItem si;
        si.types = addr.types;
        si.value = addr.toString();
        addValue(si, true);
    }
    foreach (const PostalAddress& addr, rightData) {
        TypedStringItem si;
        si.types = addr.types;
        si.value = addr.toString();
        addValue(si, false);
    }
    highlightDiff(leftData!=rightData);
    buildOneItemButtons(2);
}

void PostalAddressPair::getData(QList<PostalAddress> &leftData, QList<PostalAddress> &rightData)
{
    leftData.clear();
    for (int i=0; i<count(true); i++) {
        PostalAddress p;
        getValue(i, p, true);
        leftData << p;
    }
    rightData.clear();
    for (int i=0; i<count(false); i++) {
        PostalAddress p;
        getValue(i, p, false);
        rightData << p;
    }
}

bool PostalAddressPair::checkDiff()
{
    QList<PostalAddress> leftData, rightData;
    getData(leftData, rightData);
    return leftData!=rightData;
}

bool PostalAddressPair::getValue(int index, PostalAddress &item, bool fromLeft)
{
    TypedStringItem si;
    TypedPair::getValue(index, si, fromLeft);
    item = PostalAddress::fromString(si.value, si.types);
    return true;
}

PhotoPair::PhotoPair(const QString &title, QGridLayout *layout)
    :ItemPair(title, layout, false)
{}

void PhotoPair::setData(const Photo &leftData, const Photo &rightData)
{
    photoLeft = leftData;
    photoRight = rightData;
    fillPhoto(layLeft, leftData, &lbLeft);
    fillPhoto(layRight, rightData, &lbRight);
    highlightDiff(!(leftData==rightData));
}

void PhotoPair::getData(Photo &leftData, Photo &rightData)
{
    leftData = photoLeft;
    rightData = photoRight;
}

void PhotoPair::copyData(bool toLeft)
{
    QLabel* lbDest = toLeft ? lbLeft : lbRight;
    if (toLeft)
        photoLeft = photoRight;
    else
        photoRight = photoLeft;
    showPhoto(photoLeft, lbDest);
}

bool PhotoPair::checkDiff()
{
    return (!(photoLeft==photoRight));
}

void PhotoPair::fillPhoto(QGridLayout *layout, const Photo &data, QLabel** lb)
{
    *lb = new QLabel();
    (*lb)->setAlignment(Qt::AlignHCenter);
    layout->addWidget(*lb);
    showPhoto(data, *lb);
}

