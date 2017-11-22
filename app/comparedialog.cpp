/* Double Contact
 *
 * Module: Contact comparison dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QFrame>

#include "comparedialog.h"
#include "ui_comparedialog.h"

CompareDialog::CompareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompareDialog),
    pFullName(0), pNames(0)
{
    ui->setupUi(this);
    pFullName = 0;
    pNames = 0;
    pPhones = 0;
    pEmails = 0;
    pBDay = 0;
    pAnn = 0;
    pDesc = 0;
    pSortString = 0;
    pPhoto = 0;
    pOrg = 0;
    pTitle = 0;
    pAddrs = 0;
    pNickName = 0;
    pURL = 0;
    pIMs = 0;
}

CompareDialog::~CompareDialog()
{
    delete ui;
}

void CompareDialog::setHeaders(const QString &left, const QString &right)
{
    ui->lbLeft->setText(left);
    ui->lbRight->setText(right);
}

template<class P, class T>
void CompareDialog::checkPair(const QString& title, P** pair, T& leftData, T& rightData)
{
    *pair = 0;
    if (!leftData.isEmpty() || !rightData.isEmpty()) {
        *pair = new P(title, ui->layPairs);
        (*pair)->setData(leftData, rightData);
    }
}

void CompareDialog::setData(const ContactItem &left, const ContactItem &right)
{
    ui->layPairs->setAlignment(Qt::AlignTop);
    checkPair(S_FULL_NAME, &pFullName, left.fullName, right.fullName);
    checkPair(tr("Names"), &pNames, left.names, right.names);
    checkPair(tr("Phones"), &pPhones, left.phones, right.phones);
    checkPair(tr("Emails"), &pEmails, left.emails, right.emails);
    checkPair(S_BDAY, &pBDay, left.birthday, right.birthday);
    checkPair(tr("Anniversaries"), &pAnn, left.anniversary, right.anniversary);
    checkPair(S_DESC, &pDesc, left.description, right.description);
    checkPair(tr("Sort string"), &pSortString, left.sortString, right.sortString);
    checkPair(S_PHOTO, &pPhoto, left.photo, right.photo);
    checkPair(S_ORG, &pOrg, left.organization, right.organization);
    checkPair(S_TITLE, &pTitle, left.title, right.title);
    checkPair(tr("Addresses"), &pAddrs, left.addrs, right.addrs);
    checkPair(S_NICK, &pNickName, left.nickName, right.nickName);
    checkPair(S_URL, &pURL, left.url, right.url);
    checkPair(S_IM, &pIMs, left.ims, right.ims);
}

void CompareDialog::getData(ContactItem &left, ContactItem &right)
{
    if (pFullName)
        pFullName->getData(left.fullName, right.fullName);
    if (pNames)
        pNames->getData(left.names, right.names);
    if (pPhones)
        pPhones->getData(left.phones, right.phones);
    if (pEmails)
        pEmails->getData(left.emails, right.emails);
    if (pBDay)
        pBDay->getData(left.birthday, right.birthday);
    if (pAnn)
        pAnn->getData(left.anniversary, right.anniversary);
    if (pDesc)
        pDesc->getData(left.description, right.description);
    if (pSortString)
        pSortString->getData(left.sortString, right.sortString);
    if (pPhoto)
        pPhoto->getData(left.photo, right.photo);
    if (pOrg)
        pOrg->getData(left.organization, right.organization);
    if (pTitle)
        pTitle->getData(left.title, right.title);
    if (pAddrs)
        pAddrs->getData(left.addrs, right.addrs);
    if (pNickName)
        pNickName->getData(left.nickName, right.nickName);
    if (pURL)
        pURL->getData(left.url, right.url);
    if (pIMs)
        pIMs->getData(left.ims, right.ims);
}
