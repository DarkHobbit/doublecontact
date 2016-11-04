/* Double Contact
 *
 * Module: Multi contact editing dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "multicontactdialog.h"
#include "ui_multicontactdialog.h"

MultiContactDialog::MultiContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiContactDialog)
{
    ui->setupUi(this);
}

MultiContactDialog::~MultiContactDialog()
{
    delete ui;
}

void MultiContactDialog::setData(const ContactItem &item, bool firstItem)
{
    if (firstItem) {
        ui->leTitle->setText(item.title);
        ui->leOrganization->setText(item.organization);
        ui->edDescription->setPlainText(item.organization);
    }
    else {
        if (ui->leTitle->text()!=item.title)
            ui->leTitle->setText("*");
        if (ui->leOrganization->text()!=item.organization)
            ui->leOrganization->setText("*");
        if (ui->edDescription->toPlainText()!=item.description)
            ui->edDescription->setPlainText("*");
    }
}

void MultiContactDialog::getData(ContactItem &item)
{
    if (ui->leTitle->text()!=("*"))
        item.title = ui->leTitle->text();
    if (ui->leOrganization->text()!=("*"))
        item.organization = ui->leOrganization->text();
    if (ui->edDescription->toPlainText()!=("*"))
        item.description = ui->edDescription->toPlainText();
}
