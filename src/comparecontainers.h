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
    ItemPair(const QString& title, QGridLayout* layout);
protected:
    QGroupBox* gbLeft;
    QGroupBox* gbRight;
    virtual void copyData(bool toLeft)=0;
    void highlightDiff(bool hasDiff);
private:
    QToolButton* btnToLeft;
    QToolButton* btnToRight;
private slots:
    void onToLeftClicked();
    void onToRightClicked();
};

class StringListPair: public ItemPair
{
    Q_OBJECT
public:
    StringListPair(const QString& title, QGridLayout* layout, const QStringList& leftData, const QStringList& rightData);
    void getData(QStringList& leftData, QStringList& rightData);
protected:
    virtual void copyData(bool toLeft);
private:
    QList<QLineEdit*> leftSet;
    QList<QLineEdit*> rightSet;
    void fillBox(QGroupBox* gBox, const QStringList& data, QList<QLineEdit*>& edSet);
};

class StringPair: public StringListPair
{
    Q_OBJECT
public:
    StringPair(const QString& title, QGridLayout* layout, const QString& leftData, const QString& rightData);
    void getData(QString& leftData, QString& rightData);
};

class TypedPair: public ItemPair
{
    Q_OBJECT
public:
    TypedPair(const QString& title, QGridLayout* layout);
protected:
    void addValue(const QString& value, const QStringList& types, bool toLeft);
    int count(bool onLeft);
    bool getValue(int index, QString& value, QStringList& types, bool fromLeft);
    virtual void copyData(bool toLeft);
private:
    QGridLayout* layLeft;
    QGridLayout* layRight;
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
    void getData(QList<Phone>& leftPhones, QList<Phone>& rightPhones);
};

class EmailsPair: public TypedPair
{
    Q_OBJECT
public:
    EmailsPair(const QString& title, QGridLayout* layout, const QList<Email>& leftEmails, const QList<Email>& rightEmails);
    void getData(QList<Email>& leftEmails, QList<Email>& rightEmails);
};

// TODO DateItemPair, AnnPair (how to unify?..) and AddressPair

#endif // COMPARECONTAINERS_H
