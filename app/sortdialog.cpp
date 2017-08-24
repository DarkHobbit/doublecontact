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
#include "globals.h"
#include "sortdialog.h"
#include "ui_sortdialog.h"

SortDialog::SortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SortDialog)
{
    ui->setupUi(this);
    ui->rbBySortString->setText(tr("Sort string"));
    ui->rbByLastName->setText(S_LAST_NAME);
    ui->rbByFirstName->setText(S_FIRST_NAME);
    ui->rbByFullName->setText(S_FULL_NAME);
    ui->rbByNick->setText(S_NICK);
    ui->rbByGroup->setText(S_GROUP);
}

SortDialog::~SortDialog()
{
    delete ui;
}

ContactList::SortType SortDialog::getData()
{
    if (ui->rbBySortString->isChecked())
        return ContactList::SortBySortString;
    else if (ui->rbByLastName->isChecked())
        return ContactList::SortByLastName;
    else if (ui->rbByFirstName->isChecked())
        return ContactList::SortByFirstName;
    else if (ui->rbByFullName->isChecked())
        return ContactList::SortByFullName;
    else if (ui->rbByNick->isChecked())
        return ContactList::SortByNick;
    else
        return ContactList::SortByGroup;
}

void SortDialog::setData(ContactList::SortType sortType)
{
    if (sortType==ContactList::SortBySortString)
        ui->rbBySortString->setChecked(true);
    else if (sortType==ContactList::SortByLastName)
        ui->rbByLastName->setChecked(true);
    else if (sortType==ContactList::SortByFirstName)
        ui->rbByFirstName->setChecked(true);
    else if (sortType==ContactList::SortByFullName)
        ui->rbByFullName->setChecked(true);
    else if (sortType==ContactList::SortByNick)
        ui->rbByNick->setChecked(true);
    else
        ui->rbByGroup->setChecked(true);
}
