/* Double Contact
 *
 * Module: Phone type selection dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QGridLayout>
#include <QCheckBox>
#include <QStringList>
#include "contactlist.h"
#include "phonetypedialog.h"
#include "ui_phonetypedialog.h"

#define COLUMN_COUNT 3
PhoneTypeDialog::PhoneTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhoneTypeDialog)
{
    ui->setupUi(this);
    // Fill dialog by all available types
    short i=0;
    foreach (const QString& s, fullPhoneFlagSet.typeToStrList()) {
        short row = i / COLUMN_COUNT;
        short col = i % COLUMN_COUNT;
        QCheckBox* cb = new QCheckBox(s);
        cb->setObjectName(QString("cb%1").arg(i));
        ui->layTypes->addWidget(cb, row, col);
        i++;
    }
}

PhoneTypeDialog::~PhoneTypeDialog()
{
    delete ui;
}

QString PhoneTypeDialog::getData() const
{
    QStringList selectedTypes;
    foreach(QObject* obj, this->children()) {
        QCheckBox* cb = dynamic_cast<QCheckBox*>(obj);
        if (cb)
            if (cb->isChecked())
                selectedTypes.push_back(cb->text());
    }
    return selectedTypes.join("+");
}
