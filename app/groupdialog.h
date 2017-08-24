/* Double Contact
 *
 * Module: Groups dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef GROUPDIALOG_H
#define GROUPDIALOG_H

#include <QDialog>
#include <QShortcut>
#include "contactmodel.h"

namespace Ui {
class GroupDialog;
}

class GroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupDialog(ContactModel& _model);
    ~GroupDialog();
protected:
    void showEvent(QShowEvent*);
private slots:
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnRemove_clicked();
    void on_btnMerge_clicked();
    void on_btnSplit_clicked();
    void rowDoubleClicked(const QModelIndex&);
private:
    Ui::GroupDialog *ui;
    ContactModel& model;
    bool askGroupName(QString& name);
    void updateTable(const QString& selectedGroup = "");
    bool checkSelection();
};

#endif // GROUPDIALOG_H
