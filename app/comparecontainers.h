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

#ifndef COMPARECONTAINERS_H
#define COMPARECONTAINERS_H

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QStringList>
#include <QToolButton>
#include "contactlist.h"

class ItemPair: public QObject
{
    Q_OBJECT
public:
    ItemPair(const QString& title, QGridLayout* layout, bool multiItem);
protected:
    bool _multiItem;
    QGridLayout* layLeft;
    QGridLayout* layRight;
    virtual void copyData(bool toLeft)=0;
    virtual void copyOneItem(bool toLeft, int srcIndex)=0;
    virtual bool checkDiff()=0;
    void buildOneItemButtonSide(bool toLeft, int column);
    void buildOneItemButtons(int column);
    void addOneItemButton(bool toLeft, int column);
    QLineEdit* addEditor(const QString& text);
    QComboBox* addCombo();
    void highlightDiff(bool hasDiff);
private:
    QGroupBox* gbLeft;
    QGroupBox* gbRight;
    QToolButton* btnToLeft;
    QToolButton* btnToRight;
    QList<QToolButton*> btnsOneItemToLeft;
    QList<QToolButton*> btnsOneItemToRight;
private slots:
    void onItemChanged();
    void onToLeftClicked();
    void onToRightClicked();
    void onOneItemToLeftClicked();
    void onOneItemToRightClicked();
};

class StringListPair: public ItemPair
{
    Q_OBJECT
public:
    StringListPair(const QString& title, QGridLayout* layout,
        const QStringList& leftData, const QStringList& rightData, bool multiItem = true);
    void getData(QStringList& leftData, QStringList& rightData);
protected:
    virtual void copyData(bool toLeft);
    virtual void copyOneItem(bool toLeft, int srcIndex);
    virtual bool checkDiff();
private:
    QList<QLineEdit*> leftSet;
    QList<QLineEdit*> rightSet;
    void fillBox(QGridLayout* layout, const QStringList& data, QList<QLineEdit*>& edSet);
};

class StringPair: public StringListPair
{
    Q_OBJECT
public:
    StringPair(const QString& title, QGridLayout* layout,
        const QString& leftData, const QString& rightData);
    void getData(QString& leftData, QString& rightData);
};

class TypedPair: public ItemPair
{
    Q_OBJECT
public:
    TypedPair(const QString& title, QGridLayout* layout);
    virtual ~TypedPair();
    template<class T>
    void getData(QList<T>& leftItems, QList<T>& rightItems);
protected:
    StandardTypes* standardTypes;
    void addValue(const TypedDataItem& item, bool toLeft);
    int count(bool onLeft);
    bool getValue(int index, TypedDataItem& item, bool fromLeft);
    virtual void copyData(bool toLeft);
    virtual void copyOneItem(bool toLeft, int srcIndex);
private:
    QList<QLineEdit*> leftEdSet;
    QList<QComboBox*> leftComboSet;
    QList<QLineEdit*> rightEdSet;
    QList<QComboBox*> rightComboSet;
};

class PhonesPair: public TypedPair
{
    Q_OBJECT
public:
    PhonesPair(const QString& title, QGridLayout* layout, const QList<Phone>& leftPhones, const QList<Phone>& rightPhones);
protected:
    virtual bool checkDiff();
};

class EmailsPair: public TypedPair
{
    Q_OBJECT
public:
    EmailsPair(const QString& title, QGridLayout* layout, const QList<Email>& leftEmails, const QList<Email>& rightEmails);
protected:
    virtual bool checkDiff();
 };

class DateItemListPair: public ItemPair
{
    Q_OBJECT
public:
    DateItemListPair(const QString& title, QGridLayout* layout,
        const QList<DateItem>& leftData, const QList<DateItem>& rightData, bool multiItem = true);
    void getData(QList<DateItem>& leftData, QList<DateItem>& rightData);
protected:
    virtual void copyData(bool toLeft);
    virtual void copyOneItem(bool toLeft, int srcIndex);
    virtual bool checkDiff();
private:
    QList<QLabel*> leftSet;
    QList<QLabel*> rightSet;
    QList<DateItem> leftDataSet;
    QList<DateItem> rightDataSet;
    void fillBox(QGridLayout* layout, const QList<DateItem>& data, QList<QLabel*>& lbSet);
};

class DateItemPair: public DateItemListPair
{
    Q_OBJECT
public:
    DateItemPair(const QString& title, QGridLayout* layout,
        const DateItem& leftData, const DateItem& rightData);
    void getData(DateItem& leftData, DateItem& rightData);
};

class PostalAddressPair: public StringListPair
{
    Q_OBJECT
public:
    PostalAddressPair(const QString& title, QGridLayout* layout,
        const PostalAddress& leftData, const PostalAddress& rightData);
    void getData(PostalAddress& leftData, PostalAddress& rightData);
};

#endif // COMPARECONTAINERS_H
