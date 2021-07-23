/* Double Contact
 *
 * Module: Call History Window
 *
 * Copyright 2021 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QShortcut>
#include <QStringList>
#include <QTemporaryFile>
#include <QUrl>

#include "callwindow.h"
#include "ui_callwindow.h"
#include "configmanager.h"
#include "helpers.h"

CallWindow::CallWindow(ContactList* contacts) :
    QDialog(0),
    ui(new Ui::CallWindow),
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
    ui->tvCalls->horizontalHeader()->setStretchLastSection(true);
    updateTableConfig(ui->tvCalls);  
    // Model
    model = new CallModel(this, contacts);
    proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(-1);
    proxy->setSortRole(SortStringRole);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive); // Driver == driver
    ui->tvCalls->setModel(proxy);
    connect(ui->tvCalls->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));
    connect(ui->tvCalls->selectionModel(), SIGNAL(currentChanged(QItemIndex,QItemIndex)),
            this, SLOT(selectionChanged()));
    connect(ui->tvCalls->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged()));
    // Shortcuts
    QShortcut* shcSortToggle = new QShortcut(Qt::Key_F4, this);
    connect(shcSortToggle, SIGNAL(activated()), this, SLOT(toggleSort()));
    // Context menu
    ui->tvCalls->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tvCalls->addAction(ui->actionCopy_text);
    // Calc!
    updateModel();
    ui->tvCalls->resizeColumnsToContents();
    readTableSortConfig(ui->tvCalls, false);
}

CallWindow::~CallWindow()
{
    delete ui;
}

QModelIndex CallWindow::selectedRecord(bool errorIfNoSelected)
{
    // check Selection
    QModelIndexList proxySelection = ui->tvCalls->selectionModel()->selectedRows();
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

void CallWindow::updateModel()
{
    model->update();
    lbCount->setText(tr("Records: %1").arg(model->rowCount()));
    updateStatus();
}

void CallWindow::updateStatus()
{
    lbMode->setText(SS_MODE +
                    (ui->tvCalls->isSortingEnabled() ? SS_SORT_ON : SS_SORT_OFF));
}

void CallWindow::toggleSort()
{
    bool needSort = !ui->tvCalls->isSortingEnabled();
    readTableSortConfig(ui->tvCalls, true, needSort);
    writeTableSortConfig(ui->tvCalls);
    updateStatus();
}

void CallWindow::onSortIndicatorChanged(int, Qt::SortOrder)
{
    QHeaderView* header = dynamic_cast<QHeaderView*>(sender());
    if (header)
        writeTableSortConfig(header);
}

void CallWindow::on_actionCopy_text_triggered()
{
    QModelIndex ind = ui->tvCalls->selectionModel()->currentIndex();
    ind = proxy->mapToSource(ind);
    QString text = model->data(ind, Qt::DisplayRole).toString();
    qApp->clipboard()->setText(text);
}

void CallWindow::on_leFilter_textChanged(const QString &newText)
{
    if (newText.isEmpty())
        proxy->setFilterWildcard("");
    else
        proxy->setFilterWildcard(newText);
}

void CallWindow::on_btnSaveAs_clicked()
{
    QFileInfo fi(configManager.lastContactFile());
    QString path = fi.path() + QDir::separator() + fi.baseName() + ".csv";
    QString selectedFilter;
    path = QFileDialog::getSaveFileName(0, tr("Save calls file"),
        path, "*.csv",
        &selectedFilter);
    if (!path.isEmpty())
        model->saveToCSV(path);
}

void CallWindow::showEvent(QShowEvent *)
{
    ui->tvCalls->resizeRowsToContents();
    ui->tvCalls->selectRow(0);
}
