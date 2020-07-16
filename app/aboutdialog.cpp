/* Double Contact
 *
 * Module: About dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->lwContributors->addItem("Icons made by Freepik from www.flaticon.com");
    ui->lwContributors->addItem("Brazilian Portuguese UI (work in progress): BrunaKimura");
//    ui->lwContributors->addItem("Chinese (Traditional) UI: Louies");
    ui->lwContributors->addItem("Dutch UI: Heimen Stoffels aka Vistaus");
    ui->lwContributors->addItem("French UI: Nathan Bonnemains aka NathanBnm");
    ui->lwContributors->addItem("German UI: Andreas Kleinert aka andyk");
    ui->lwContributors->addItem(QString::fromUtf8("Norwegian (Bokmål) UI: Allan Nordhøy aka comradekingu"));
    ui->lwContributors->addItem("Portuguese UI (work in progress): Manuela Silva aka mansil, alecosta19");
    ui->lwContributors->addItem("Ukrainian UI: Olexandr Nesterenko aka burunduk");
    ui->lwContributors->addItem("NBU files support ported from NbuExplorer project");
    ui->lwContributors->addItem("(Author: Petr Vilem, petrusek@seznam.cz)");
    ui->lwContributors->addItem("Bugfixes: antongus, GitKroz, SauronfromMordor");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
