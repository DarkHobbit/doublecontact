/* Double Contact
 *
 * Module: Mass tag remove dialog
 *
 * Copyright 2021 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef TAGREMOVEDIALOG_H
#define TAGREMOVEDIALOG_H

#include <QDialog>
#include "contactlist.h"

namespace Ui {
class TagRemoveDialog;
}

class TagRemoveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagRemoveDialog(QWidget *parent = 0);
    ~TagRemoveDialog();
    bool setData(ContactList& contacts);
    QStringList getData();
private slots:
    void on_btnSelectAll_clicked();

private:
    Ui::TagRemoveDialog *ui;
};

#endif // TAGREMOVEDIALOG_H
