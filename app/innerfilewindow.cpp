/* Double Contact
 *
 * Module: Inner Filesystem Window
 *
 * Copyright 2022 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QShortcut>
#include <QStringList>
#include <QTemporaryFile>
#include <QUrl>

#include "innerfilewindow.h"
#include "ui_innerfilewindow.h"
#include "configmanager.h"
#include "helpers.h"

InnerFileWindow::InnerFileWindow(ContactList* contacts) :
    QDialog(0),
    ui(new Ui::InnerFileWindow),
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
    ui->tvFiles->horizontalHeader()->setStretchLastSection(true);
    // Model
    model = new InnerFileModel(this, contacts);
    proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(-1);
    proxy->setSortRole(SortStringRole);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive); // Driver == driver
    ui->tvFiles->setModel(proxy);
    connect(ui->tvFiles->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));
    // Shortcuts
    QShortcut* shcSortToggle = new QShortcut(Qt::Key_F4, this);
    connect(shcSortToggle, SIGNAL(activated()), this, SLOT(toggleSort()));
    // Context menu
    ui->tvFiles->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tvFiles->addAction(ui->actionCopy_text);
    ui->tvFiles->addAction(ui->actionSave_file);
    ui->tvFiles->addAction(ui->actionShowFile);
    // Calc!
    updateModel();
    ui->tvFiles->resizeColumnsToContents();
    readTableSortConfig(ui->tvFiles, false);
}

InnerFileWindow::~InnerFileWindow()
{
    delete ui;
}

QModelIndex InnerFileWindow::selectedRecord(bool errorIfNoSelected)
{
    // check Selection
    QModelIndexList proxySelection = ui->tvFiles->selectionModel()->selectedRows();
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

void InnerFileWindow::updateModel()
{
    model->update();
    lbCount->setText(S_FILES_STATUS
        .arg(model->rowCount())
        .arg(model->totalSize()));
    updateStatus();
}

void InnerFileWindow::updateStatus()
{
    lbMode->setText(SS_MODE +
                    (ui->tvFiles->isSortingEnabled() ? SS_SORT_ON : SS_SORT_OFF));
}

void InnerFileWindow::toggleSort()
{
    bool needSort = !ui->tvFiles->isSortingEnabled();
    readTableSortConfig(ui->tvFiles, true, needSort);
    writeTableSortConfig(ui->tvFiles);
    updateStatus();
}

void InnerFileWindow::onSortIndicatorChanged(int, Qt::SortOrder)
{
    QHeaderView* header = dynamic_cast<QHeaderView*>(sender());
    if (header)
        writeTableSortConfig(header);
}

void InnerFileWindow::on_actionCopy_text_triggered()
{
    QModelIndex ind = ui->tvFiles->selectionModel()->currentIndex();
    ind = proxy->mapToSource(ind);
    QString text = model->data(ind, Qt::DisplayRole).toString();
    qApp->clipboard()->setText(text);
}

void InnerFileWindow::on_leFilter_textChanged(const QString &newText)
{
    if (newText.isEmpty())
        proxy->setFilterWildcard("");
    else
        proxy->setFilterWildcard(newText);
}

void InnerFileWindow::on_btnSaveAll_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,
        S_ATT_FILES_DIR,
        configManager.lastContactFile(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        QString fatalError;
        if (!model->saveAll(path, fatalError))
            QMessageBox::critical(0, S_ERROR, fatalError);
    }
}

void InnerFileWindow::showEvent(QShowEvent *)
{
    updateTableConfig(ui->tvFiles);
    ui->tvFiles->selectRow(0);
}

void InnerFileWindow::on_actionShowFile_triggered()
{
    QModelIndex sel = selectedRecord(false);
    if (!sel.isValid())
        return;
    const InnerFile& f = model->item(sel.row());
    showInnerFile(f);
}


void InnerFileWindow::on_actionSave_file_triggered()
{
    QModelIndex sel = selectedRecord(false);
    if (!sel.isValid())
        return;
    const InnerFile& f = model->item(sel.row());
    QFileInfo fi(configManager.lastContactFile());
    QString path = fi.path() + QDir::separator() + f.name;
    QString selectedFilter;
    path = QFileDialog::getSaveFileName(0, S_SAVE_ATT,
        path, "*.*",
        &selectedFilter);
    if (!path.isEmpty()) {
        QString fatalError;
        if (!f.saveAs(path, fatalError))
            QMessageBox::critical(0, S_ERROR, fatalError);
    }
}

