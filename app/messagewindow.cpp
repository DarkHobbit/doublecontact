/* Double Contact
 *
 * Module: SMS Window
 *
 * Copyright 2020 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QClipboard>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QShortcut>
#include <QStringList>

#include "messagewindow.h"
#include "ui_messagewindow.h"
#include "configmanager.h"
#include "helpers.h"
#include "logwindow.h"

MessageWindow::MessageWindow(ContactList* contacts) :
    QDialog(0),
    ui(new Ui::MessageWindow),
    model(0)
{
    ui->setupUi(this);
    // Status bar
    statusBar = new QStatusBar(this);
    lbCount = new QLabel(0);
    lbMode = new QLabel(0);
    statusBar->addWidget(lbCount);
    statusBar->addWidget(lbMode);
    layout()->addWidget(statusBar);
    // Table
    ui->tvMessages->horizontalHeader()->setStretchLastSection(true);
    updateTableConfig(ui->tvMessages);
    // Check button access
    ui->cbPDU->setEnabled(!contacts->extra.pduSMS.isEmpty());
    ui->cbPDUArchive->setEnabled(!contacts->extra.pduSMSArchive.isEmpty());
    ui->cbVmessage->setEnabled(!contacts->extra.vmsgSMS.isEmpty());
    ui->cbVmessageArchive->setEnabled(!contacts->extra.vmsgSMSArchive.isEmpty());
    ui->cbBinary->setEnabled(!contacts->extra.binarySMS.isEmpty());
    // Load conf
    configManager.readMessageViewConfig(srcFlags);
    ui->cbPDU->setChecked(srcFlags.testFlag(usePDU) && ui->cbPDU->isEnabled());
    ui->cbPDUArchive->setChecked(srcFlags.testFlag(usePDUArchive) && ui->cbPDUArchive->isEnabled());
    ui->cbVmessage->setChecked(srcFlags.testFlag(useVMessage) && ui->cbVmessage->isEnabled());
    ui->cbVmessageArchive->setChecked(srcFlags.testFlag(useVMessageArchive) && ui->cbVmessageArchive->isEnabled());
    ui->cbBinary->setChecked(srcFlags.testFlag(useBinary) && ui->cbBinary->isEnabled());
    //ui->cbMergeDups->setChecked(srcFlags.???);
    // TODO merge multipart
    checkMergeButton();
    // Model
    model = new MessageModel(this, contacts);
    proxy = new ContactSorterFilter(this);
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(-1);
    ui->tvMessages->setModel(proxy);
    ui->tvMessages->setSortingEnabled(configManager.sortingEnabled());
    updateModel();
    ui->tvMessages->resizeColumnsToContents();
    // Shortcuts
    QShortcut* shcSortToggle = new QShortcut(Qt::Key_F4, this);
    connect(shcSortToggle, SIGNAL(activated()), this, SLOT(toggleSort()));
    // Context menu
    ui->tvMessages->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tvMessages->addAction(ui->actionCopy_text);
    ui->tvMessages->addAction(ui->actionProperties);
}

MessageWindow::~MessageWindow()
{
    delete ui;
}

void MessageWindow::checkButtons()
{
    checkMergeButton();
    configManager.writeMessageViewConfig(srcFlags);
    if (model)
        updateModel();
}

void MessageWindow::checkMergeButton()
{
    short srcCount = 0;
    if (ui->cbPDU->isChecked()) srcCount++;
    if (ui->cbPDUArchive->isChecked()) srcCount++;
    if (ui->cbVmessage->isChecked()) srcCount++;
    if (ui->cbVmessageArchive->isChecked()) srcCount++;
    if (ui->cbBinary->isChecked()) srcCount++;
    /* TODO
    ui->cbMergeDups->setEnabled(srcCount>1);
    ui->cbMergeMultiparts->setEnabled(srcCount>0);
    */
}

void MessageWindow::on_cbPDU_stateChanged(int)
{
    setQFlag(srcFlags, usePDU, ui->cbPDU->isChecked());
    checkButtons();
}

void MessageWindow::on_cbPDUArchive_stateChanged(int)
{
    setQFlag(srcFlags, usePDUArchive,  ui->cbPDUArchive->isChecked());
    checkButtons();
}

void MessageWindow::on_cbBinary_stateChanged(int)
{
    setQFlag(srcFlags, useBinary,  ui->cbBinary->isChecked());
    checkButtons();
}

void MessageWindow::on_cbVmessage_stateChanged(int)
{
    setQFlag(srcFlags, useVMessage,  ui->cbVmessage->isChecked());
    checkButtons();
}

void MessageWindow::on_cbVmessageArchive_stateChanged(int)
{
    setQFlag(srcFlags, useVMessageArchive,  ui->cbVmessageArchive->isChecked());
    checkButtons();
}

void MessageWindow::on_cbMergeDups_stateChanged(int)
{
    // TODO save to conf
    // and redraw
}

void MessageWindow::updateModel()
{
    QStringList errors;
    model->update(srcFlags, errors);
    ui->tvMessages->resizeRowsToContents(); //TODO move to buttonproc and optionally return to defaults after merging multipart messages
    if (!errors.isEmpty()) {
        LogWindow* w = new LogWindow(0);
        w->setData(tr(" with messages"), model->rowCount(), errors);
        w->exec();
        delete w;
    }
    lbCount->setText(tr("Records: %1").arg(model->rowCount()));
    updateStatus();
}

void MessageWindow::updateStatus()
{
    lbMode->setText(SS_MODE +
                    (ui->tvMessages->isSortingEnabled() ? SS_SORT_ON : SS_SORT_OFF));
}

void MessageWindow::toggleSort()
{
    bool needSort = !ui->tvMessages->isSortingEnabled();
    ui->tvMessages->setSortingEnabled(needSort);
    int sortColumn = needSort ? 0 : -1;
    proxy->sort(sortColumn);
    updateStatus();
}

void MessageWindow::on_actionCopy_text_triggered()
{
    QModelIndex ind = ui->tvMessages->selectionModel()->currentIndex();
    ind = proxy->mapToSource(ind);
    QString text = model->data(ind, Qt::DisplayRole).toString();
    qApp->clipboard()->setText(text);
}

void MessageWindow::on_actionProperties_triggered()
{
    // check Selection (if needed in other places, move to func)
    QModelIndexList proxySelection = ui->tvMessages->selectionModel()->selectedRows();
    if (proxySelection.count()==0) {
        QMessageBox::critical(0, S_ERROR, S_REC_NOT_SEL);
        return;
    }
    if (proxySelection.count()>1) {
        QMessageBox::critical(0, S_ERROR, S_ONLY_ONE_REC);
        return;
    }
    // Single selection
    QModelIndex sel = proxy->mapToSource(proxySelection.first());
    const DecodedMessage& msg = model->item(sel.row());
    // Message sources
    QStringList srcs;
    if (msg.sources.testFlag(useVMessage))
        srcs << ui->cbVmessage->text();
    if (msg.sources.testFlag(useVMessageArchive))
        srcs << ui->cbVmessageArchive->text();
    if (msg.sources.testFlag(usePDU))
        srcs << ui->cbPDU->text();
    if (msg.sources.testFlag(usePDUArchive))
        srcs << ui->cbPDUArchive->text();
    if (msg.sources.testFlag(useBinary))
        srcs << ui->cbBinary->text();
    // Show info
    QString info = tr("id: %1\nSources: %2")
        .arg(msg.id).arg(srcs.join(", "));
    QMessageBox::information(0, S_INFORM, info);
}

void MessageWindow::on_leFilter_textChanged(const QString &newText)
{
    if (newText.isEmpty())
        proxy->setFilterWildcard("");
    else
        proxy->setFilterWildcard(newText);
}

void MessageWindow::on_btnSaveAs_clicked()
{
    QFileInfo fi(configManager.lastContactFile());
    QString path = fi.path() + QDir::separator() + fi.baseName() + ".csv";
    QString selectedFilter;
    path = QFileDialog::getSaveFileName(0, tr("Save messages file"),
        path, "*.csv",
        &selectedFilter);
    if (!path.isEmpty())
        model->saveToCSV(path);
}
