/* Double Contact
 *
 * Module: Date details dialog (time presence, time zone)
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "datedetailsdialog.h"
#include "ui_datedetailsdialog.h"

DateDetailsDialog::DateDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateDetailsDialog)
{
    ui->setupUi(this);
    ui->sbHour->setEnabled(false); //==>
    ui->sbMin->setEnabled(false); //==>
}

DateDetailsDialog::~DateDetailsDialog()
{
    delete ui;
}

void DateDetailsDialog::setData(const DateItem &data)
{
    ui->dteDate->setDateTime(data.value);
    ui->cbHasTime->setChecked(data.hasTime);
    if (data.hasTime) {
        ui->cbHasTimeZone->setChecked(data.hasTimeZone);
        if (data.hasTimeZone) {
            ui->sbHour->setValue(data.zoneHour);
            ui->sbMin->setValue(data.zoneMin);
        }
    }
    on_cbHasTime_toggled(data.hasTime);
}

void DateDetailsDialog::getData(DateItem &data)
{
    data.value = ui->dteDate->dateTime();
    data.hasTime = ui->cbHasTime->isChecked();
    if (data.hasTime) {
        data.hasTimeZone = ui->cbHasTimeZone->isChecked();
        if (data.hasTimeZone) {
            data.zoneHour = ui->sbHour->value();
            data.zoneMin = ui->sbMin->value();
        }
    }
}

void DateDetailsDialog::on_cbHasTime_toggled(bool checked)
{
    setDateFormat(ui->dteDate, checked);
    if (!checked)
        ui->cbHasTimeZone->setChecked(false);
    ui->cbHasTimeZone->setEnabled(checked);
    on_cbHasTimeZone_toggled(ui->cbHasTimeZone->isChecked());
}

void DateDetailsDialog::on_cbHasTimeZone_toggled(bool checked)
{
    if (!checked) {
        ui->sbHour->setValue(0);
        ui->sbMin->setValue(0);
    }
    ui->sbHour->setEnabled(checked);
    ui->sbMin->setEnabled(checked);
    ui->sbHour->setEnabled(false); //==>
    ui->sbMin->setEnabled(false); //==>
}

void DateDetailsDialog::setDateFormat(QDateTimeEdit *editor, const bool hasTime)
{
    if (hasTime)
        editor->setDisplayFormat("dd.MM.yyyy H:mm");
    else
        editor->setDisplayFormat("dd.MM.yyyy");
}

