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
#include <QItemSelectionModel>
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
    lbMMSCount = new QLabel(0);
    lbMode = new QLabel(0);
    lbDups = new QLabel(0);
    lbMultiParts = new QLabel(0);
    statusBar->addWidget(lbCount);
    statusBar->addWidget(lbMMSCount);
    statusBar->addWidget(lbMode);
    statusBar->addWidget(lbDups);
    statusBar->addWidget(lbMultiParts);
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
    ui->cbMergeDups->setChecked(srcFlags.testFlag(mergeDuplicates));
    ui->cbMergeMultiparts->setChecked(srcFlags.testFlag(mergeMultiParts));
    checkMergeButton();
    // Model
    model = new MessageModel(this, contacts);
    proxy = new ContactSorterFilter(this);
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(-1);
    ui->tvMessages->setModel(proxy);
    ui->tvMessages->setSortingEnabled(configManager.sortingEnabled());
    connect(ui->tvMessages->selectionModel(), SIGNAL(currentChanged(QItemIndex,QItemIndex)),
            this, SLOT(selectionChanged()));
    connect(ui->tvMessages->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged()));
    // Shortcuts
    QShortcut* shcSortToggle = new QShortcut(Qt::Key_F4, this);
    connect(shcSortToggle, SIGNAL(activated()), this, SLOT(toggleSort()));
    // Context menu
    ui->tvMessages->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tvMessages->addAction(ui->actionCopy_text);
    ui->tvMessages->addAction(ui->actionSave_MMS_Files);
    ui->tvMessages->addAction(ui->actionProperties);
    // Calc!
    updateModel();
    ui->tvMessages->resizeColumnsToContents();
    setSorting(configManager.sortingEnabled());
}

MessageWindow::~MessageWindow()
{
    delete ui;
}

void MessageWindow::selectionChanged()
{
    QModelIndex sel = selectedRecord(false);
    if (!sel.isValid()) {
        ui->actionProperties->setEnabled(false);
        ui->actionSave_MMS_Files->setEnabled(false);
    }
    else {
        ui->actionProperties->setEnabled(true);
        const DecodedMessage& msg = model->item(sel.row());
        ui->actionSave_MMS_Files->setEnabled(msg.isMMS);
    }
}

void MessageWindow::checkButtons()
{
    checkMergeButton();
    configManager.writeMessageViewConfig(srcFlags);
    if (model)
        updateModel();
    ui->tvMessages->resizeRowsToContents();
}

void MessageWindow::checkMergeButton()
{
    short srcCount = 0;
    if (ui->cbPDU->isChecked()) srcCount++;
    if (ui->cbPDUArchive->isChecked()) srcCount++;
    if (ui->cbVmessage->isChecked()) srcCount++;
    if (ui->cbVmessageArchive->isChecked()) srcCount++;
    if (ui->cbBinary->isChecked()) srcCount++;
    ui->cbMergeDups->setEnabled(srcCount>0);// Initially >1, but dups can be even in one source
    ui->cbMergeMultiparts->setEnabled(srcCount>0);
}

QModelIndex MessageWindow::selectedRecord(bool errorIfNoSelected)
{
    // check Selection
    QModelIndexList proxySelection = ui->tvMessages->selectionModel()->selectedRows();
    if (proxySelection.count()==0) {
        if (errorIfNoSelected)
            QMessageBox::critical(0, S_ERROR, S_REC_NOT_SEL);
        return QModelIndex();
    }
    if (proxySelection.count()>1) {
        QMessageBox::critical(0, S_ERROR, S_ONLY_ONE_REC);
        return QModelIndex();
    }
    // Single selection
    return proxy->mapToSource(proxySelection.first());
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
    setQFlag(srcFlags, mergeDuplicates,  ui->cbMergeDups->isChecked());
    checkButtons();
}

void MessageWindow::on_cbMergeMultiparts_stateChanged(int)
{
    setQFlag(srcFlags, mergeMultiParts,  ui->cbMergeMultiparts->isChecked());
    checkButtons();
}

void MessageWindow::updateModel()
{
    QStringList errors;
    model->update(srcFlags, errors);
    if (!errors.isEmpty()) {
        LogWindow* w = new LogWindow(0);
        w->setData(tr(" with messages"), model->rowCount(), errors);
        w->exec();
        delete w;
    }
    lbCount->setText(tr("Records: %1").arg(model->rowCount()));
    lbMMSCount->setText(tr("Including MMS: %1").arg(model->mmsCount()));
    lbDups->setText(tr("Merged dups: %1").arg(model->mergeDupCount()));
    lbMultiParts->setText(tr("Merged multiparted: %1").arg(model->mergeMultiPartCount()));
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
    setSorting(needSort);
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
    QModelIndex sel = selectedRecord();
    if (!sel.isValid())
        return;
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
    if (msg.isMMS)
        info += tr("\n\nMMS properties:\n%1").arg(msg.text);
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
    if (!path.isEmpty()) {
        model->saveToCSV(path);
        if (model->mmsCount()>0) {
            int btn = QMessageBox::question(0, S_CONFIRM,
                 tr("Can you also save media files from MMS (%1 messages)?").arg(model->mmsCount()),
                 QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            if (btn==QMessageBox::Yes) {
                QString path = QFileDialog::getExistingDirectory(this,
                    S_SELECT_MMS_DIR_TITLE, configManager.lastContactFile(),
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
                if (!path.isEmpty()) {
                    QString fatalError;
                    if (!model->saveAllMMSFiles(path, fatalError))
                        QMessageBox::critical(0, S_ERROR, fatalError);
                }
            }
        }
    }
}

void MessageWindow::showEvent(QShowEvent *)
{
    ui->tvMessages->resizeRowsToContents();
    ui->tvMessages->selectRow(0);
}

void MessageWindow::setSorting(bool needSort)
{
    ui->tvMessages->setSortingEnabled(needSort);
    int sortColumn = needSort ? 0 : -1;
    proxy->sort(sortColumn);
    updateStatus();
}

void MessageWindow::on_actionSave_MMS_Files_triggered()
{
    QModelIndex sel = selectedRecord();
    if (!sel.isValid())
        return;
    const DecodedMessage& msg = model->item(sel.row());
    QString path = QFileDialog::getExistingDirectory(this,
        S_SELECT_MMS_DIR_TITLE, configManager.lastContactFile(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        QString fatalError;
        if (!msg.saveMMSFiles(path, fatalError))
            QMessageBox::critical(0, S_ERROR, fatalError);
    }
}
