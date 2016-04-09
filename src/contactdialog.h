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

#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include "contactlist.h"

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = 0);
    ~ContactDialog();
    void clearData();
    void setData(const ContactItem& c);
    void getData(ContactItem& c);
    // TODO checkData
protected:
    void changeEvent(QEvent *e);

private:
    Ui::ContactDialog *ui;
    int nameCount, phoneCount, emailCount;
    void fillPhoneTypes(QComboBox* combo);
    void fillEmailTypes(QComboBox* combo);
    void addName(const QString& name);
    void addPhone(const Phone& ph);
    void addEmail(const Email& em);
    // Common helpers for phone/email
    void addTriplet(int& count, QGridLayout* l, const QString& nameTemplate, const QString& itemValue);
    void readTriplet(const QString& nameTemplate, int num, QString& itemValue, QStringList& types, const  ::StandardTypes& sTypes);
    void delTriplet(int& count, const QString& nameTemplate, int num);
    inline QLineEdit* nameEditorByNum(int num);
    inline QLineEdit* editorByNum(const QString& nameTemplate, int num);
private slots:
    void slotDelName();
    void slotDelTriplet();
    void on_btnAdd_clicked();
    void itemTypeChanged(const QString& value);
    void on_cbBirthday_toggled(bool checked);
};

#endif // CONTACTDIALOG_H
