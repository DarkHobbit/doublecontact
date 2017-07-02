/* Double Contact
 *
 * Module: Hard sorting parameters dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef SORTDIALOG_H
#define SORTDIALOG_H

#include <QDialog>
#include "contactlist.h"

namespace Ui {
class SortDialog;
}

class SortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SortDialog(QWidget *parent = 0);
    ~SortDialog();
    ContactList::SortType getData();
    void setData(ContactList::SortType sortType);
private:
    Ui::SortDialog *ui;
};

#endif // SORTDIALOG_H
