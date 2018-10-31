/* Double Contact
 *
 * Module: CSV profile selection dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "csvprofiledialog.h"
#include "ui_csvprofiledialog.h"

#include "configmanager.h"
#include "globals.h"
#include "languagemanager.h"

CSVProfileDialog::CSVProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVProfileDialog)
{
    ui->setupUi(this);
    setWindowTitle(S_CSV_PROFILE_SELECT);
}

CSVProfileDialog::~CSVProfileDialog()
{
    delete ui;
}

void CSVProfileDialog::selectProfile(CSVFile *format)
{
    ui->cbProfile->addItems(format->availableProfiles());
    QString profile, genEncoding, genSeparator;
    configManager.csvConfig(profile, genEncoding, genSeparator);
    // Profile
    if (!profile.isEmpty())
        ui->cbProfile->setCurrentIndex(ui->cbProfile->findText(profile));
    // Encoding list
	QStringList codecs = LanguageManager::availableCodecs();
	codecs.sort();
	ui->cbEncoding->addItems(codecs);
    // Encoding/separator (only for generic profile)
    if (profile==S_GENERIC_CSV_PROFILE) {
        if (!genEncoding.isEmpty())
            ui->cbEncoding->setCurrentIndex(ui->cbEncoding->findText(genEncoding));
        if (!genSeparator.isEmpty())
            ui->cbSeparator->setCurrentIndex(ui->cbSeparator->findText(genSeparator));
    }
    on_cbProfile_currentIndexChanged(0);
    exec();
    if (result()==QDialog::Accepted) {
        format->setProfile(ui->cbProfile->currentText());
        configManager.setCSVConfig(
            ui->cbProfile->currentText(),
            ui->cbEncoding->currentText(),
            ui->cbSeparator->currentText());
        if (profile==S_GENERIC_CSV_PROFILE) {
            format->setEncoding(ui->cbEncoding->currentText());
            format->setSeparator(ui->cbSeparator->currentText());
        }
    }
}

void CSVProfileDialog::on_cbProfile_currentIndexChanged(int)
{
    bool isGeneric = (ui->cbProfile->currentText()==S_GENERIC_CSV_PROFILE);
    ui->cbEncoding->setEnabled(isGeneric);
    ui->cbSeparator->setEnabled(isGeneric);
    if (isGeneric) {
        if (ui->cbEncoding->currentIndex()==-1)
            ui->cbEncoding->setCurrentIndex(0);
        if (ui->cbSeparator->currentIndex()==-1)
            ui->cbSeparator->setCurrentIndex(0);
    }
    else {
        ui->cbEncoding->setCurrentIndex(-1);
        ui->cbSeparator->setCurrentIndex(-1);
    }
}
