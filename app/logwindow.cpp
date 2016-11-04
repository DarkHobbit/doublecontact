/* Double Contact
 *
 * Module: Export/Import log window
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QApplication>
#include <QClipboard>
#include <QPushButton>

#include "logwindow.h"
#include "ui_logwindow.h"

LogWindow::LogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);
    // Hack to set button from box as default
    if (ui->buttonBox->buttons().count()>0) {
        QAbstractButton* abtnClose = ui->buttonBox->buttons()[0];
        QPushButton* btnClose = dynamic_cast<QPushButton*>(abtnClose);
        if (btnClose)
            btnClose->setDefault(true);
    }
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::setData(const QString& path, const ContactList &items, const QStringList &errors)
{
    setWindowTitle(tr("File %1").arg(path));
    ui->lbSummary->setText(tr("%1 records has been read").arg(items.count()));
    ui->lwErrors->clear();
    ui->lwErrors->addItems(errors);
}

void LogWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LogWindow::on_btnToClipboard_clicked()
{
    QString content = "";
    for (int i=0; i<ui->lwErrors->count(); i++) {
        if (i>0)
            content += "\n";
        content += ui->lwErrors->item(i)->text();
    }
    qApp->clipboard()->setText(content);
}
