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

#include <QByteArray>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialog>
#include <QGridLayout>
#include <QMenu>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>
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
    void setData(const ContactItem& c, const ContactList& l);
    void getData(ContactItem& c, ContactList& l);
protected:
    void changeEvent(QEvent *e);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void showEvent(QShowEvent*);
private:
    Ui::ContactDialog *ui;
    QGridLayout* layAnniversaries;
    QVBoxLayout* layAddrs;
    QMenu* menuPhotoEdit;
    int nameCount, phoneCount, emailCount, anniversaryCount, addrCount, imCount;
    DateItem birthdayDetails;
    DateItem anniversaryDetails;
    Photo photo;
    void fillPhoneTypes(QComboBox* combo);
    void fillEmailTypes(QComboBox* combo);
    void fillAddrTypes(QComboBox* combo);
    void fillIMTypes(QComboBox* combo);
    void addName(const QString& name);
    void addPhone(const Phone& ph);
    void addEmail(const Email& em);
    void addAddress(const PostalAddress& addr);
    /*
    void setAddress(QWidget* parent, const PostalAddress& addr); // TODO if more 2 addresses will allowed, need post layout and num instead this
    */
    void readAddress(int num, PostalAddress& addr);
    void addIM(const Messenger& im);
    // Common helpers for phone/email
    void addTriplet(int& count, QGridLayout* l, const QString& nameTemplate, const QString& itemValue);
    void readTriplet(const QString& nameTemplate, int num, TypedStringItem& item, const  ::StandardTypes& sTypes);
    void delTriplet(int& count, const QString& nameTemplate, int num);
    // Other common helpers
    QToolButton* addDelButton
        (int count, const QString& nameTemplate, const char* method/*, QGridLayout* l, int pos*/);
    void addTypeList(int count, const QString& nameTemplate, const QStringList& types, const ::StandardTypes& sTypes);
    void readTypelist(const QString& nameTemplate, int num, QStringList& types, const  ::StandardTypes& sTypes);
    void setDefaultTypeList(const QString& nameTemplate, int num, const QStringList& types);
    inline QLineEdit* nameEditorByNum(int num);
    inline QLineEdit* tripletEditorByNum(const QString& nameTemplate, int num);
    inline QToolButton* delNameButtonByNum(int num);
    inline QToolButton* delTripletButtonByNum(const QString& nameTemplate, int num);
    inline QComboBox* tripletTypeListByNum(const QString& nameTemplate, int num);
    void editDateDetails(QDateTimeEdit* editor, DateItem& details);
    void fixCount(int& count, const QString& nameTemplate, int minVisibleEditors);
    // Photo helpers
    void updatePhotoMenu();
    // Tag list helpers
    void buildContextMenu(QTableView* view);
    void setTagTable(const TagList& tags, QTableWidget * table);
    void getTagTable(TagList& tags, QTableWidget * table);
    void removeTag(QTableWidget* table);
    void copyTagText(QTableWidget* table);
private slots:
    void slotDelName();
    void slotDelTriplet();
    void on_btnAdd_clicked();
    void itemTypeChanged(const QString& value);
    void on_cbBirthday_toggled(bool checked);
    void on_btnBDayDetails_clicked();
    void slotDelAnniversary();
    void on_twContact_currentChanged(int);
    void on_btnAddAddress_clicked();
    void slotDelAddress();
    void onLoadImage();
    void onSaveImage();
    void onSetPhotoUrl();
    void onRemovePhoto();
    void on_btnAddIM_clicked();
    void on_btnIncludeToGroup_clicked();
    void on_btnExcludeFromGroup_clicked();
    void on_btnAnnDetails_clicked();
    void on_cbAnniversary_toggled(bool checked);
    void on_btnSaveView_clicked();
    void on_action_Remove_triggered();
    void on_actionCopy_text_triggered();
};

#endif // CONTACTDIALOG_H
