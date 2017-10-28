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

#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include "comparecontainers.h"

namespace Ui {
class CompareDialog;
}

class CompareDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CompareDialog(QWidget *parent = 0);
    ~CompareDialog();
    void setHeaders(const QString& left, const QString& right);
    void setData(const ContactItem& left, const ContactItem& right);
    void getData(ContactItem& left, ContactItem& right);
private:
    Ui::CompareDialog *ui;
    StringPair* pFullName;
    NamePair* pNames;
    PhonesPair* pPhones;
    EmailsPair* pEmails;
    DateItemPair* pBDay;
    DateItemPair* pAnn;
    StringPair *pDesc, *pSortString;
    PhotoPair* pPhoto;
    StringPair *pOrg, *pTitle;
    PostalAddressPair *pAddrs;
    StringPair *pNickName, *pURL;
    MessengersPair *pIMs;
    template<class P, class T>
        void checkPair(const QString& title, P** pair, T& leftData, T& rightData);
};

#endif // COMPAREDIALOG_H
