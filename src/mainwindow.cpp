/* Double Contact
 *
 * Module: Main Window
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    modLeft = new ContactModel(this, tr("New contact list"));
    modRight = new ContactModel(this, tr("New contact list 2"));
    proxyLeft  = new ContactSorterFilter(this);
    proxyRight  = new ContactSorterFilter(this);
    proxyLeft->setSourceModel(modLeft);
    proxyRight->setSourceModel(modRight);
    ui->tvLeft->setModel(proxyLeft);
    ui->tvRight->setModel(proxyRight);
    // Status bar
    lbMode = new QLabel(0);
    statusBar()->addWidget(lbMode);
    // Settings
    setDlg = new SettingsDialog(0);
    setDlg->readConfig();
    // Track selected view
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(anyFocusChanged(QWidget*,QWidget*)));
    connect(ui->tvLeft->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(setButtonsAccess()));
    connect(ui->tvRight->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(setButtonsAccess()));
    connect(ui->tvLeft->selectionModel(), SIGNAL(selectionChanged( const QItemSelection&, const QItemSelection&)),
            this, SLOT(setButtonsAccess()));
    connect(ui->tvRight->selectionModel(), SIGNAL(selectionChanged( const QItemSelection&, const QItemSelection&)),
            this, SLOT(setButtonsAccess()));
    connect(ui->tvLeft, SIGNAL(doubleClicked (const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));
    connect(ui->tvRight, SIGNAL(doubleClicked (const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));
    selectView(ui->tvLeft);
    selection = selectedView->selectionModel()->selectedRows();
    // Test data
    if (qApp->arguments().contains("-d"))
        modLeft->testList();
    // File command-line data
    else if (qApp->arguments().count()>1) {
        modLeft->open(qApp->arguments()[1]);
        if (qApp->arguments().count()>2) {
            ui->action_Two_panels->setChecked(true);
            modRight->open(qApp->arguments()[2]);
        }
    }
    // Previous session file data
    else if (setDlg->openLastFilesAtStartup() && QFile(setDlg->lastPath()).exists() && !(QFileInfo(setDlg->lastPath()).isDir()))
        selectedModel->open(setDlg->lastPath());
    ui->action_Two_panels->setChecked(setDlg->showTwoPanels());
    updateHeaders();
    updateMode();
    on_action_Two_panels_toggled(ui->action_Two_panels->isChecked());
}

MainWindow::~MainWindow()
{
    delete setDlg;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    askSaveChanges(event, modLeft);
    askSaveChanges(event, modRight);
}

void MainWindow::showEvent(QShowEvent*)
{
    updateConfig();
}

void MainWindow::anyFocusChanged(QWidget *, QWidget *now)
{
    if ((now==ui->tvLeft)||(now==ui->tvRight))
        selectView(dynamic_cast<QTableView*>(now));
    setButtonsAccess();
    updateHeaders();
}

void MainWindow::on_actionE_xit_triggered()
{
    close();
}

void MainWindow::on_action_Two_panels_toggled(bool showTwoPanels)
{
    if (showTwoPanels) {
        ui->tvRight->show();
        ui->lbRight->show();
    }
    else {
        ui->tvRight->hide();
        ui->lbRight->hide();
        selectView(ui->tvLeft);
    }
    setButtonsAccess();
    setDlg->setShowTwoPanels(showTwoPanels);
    updateMode();
}

void MainWindow::on_btnExit_clicked()
{
    close();
}

// Open
void MainWindow::on_action_Open_triggered()
{
    // TODO ask if model has unsaved changes (after save implementation)
    QString selectedFilter;
    QString path = QFileDialog::getOpenFileName(0, tr("Open contact file"),
        setDlg->lastPath(), FormatFactory::supportedFilters().join(";;"), &selectedFilter);
    if (!path.isEmpty()) {
        selectedModel->open(path);
        setDlg->setLastPath(path);
        updateHeaders();
    }
}

// Save
void MainWindow::on_action_Save_triggered()
{
    selectedModel->save();
}

// Save as
void MainWindow::on_actionSave_as_triggered()
{
    // TODO
}

// Add
void MainWindow::on_action_Add_triggered()
{
    ContactDialog* d = new ContactDialog(0);
    d->clearData();
    d->exec();
    if (d->result()==QDialog::Accepted) {
        ContactItem c;
        d->getData(c);
        selectedModel->addRow(c);
    }
    delete d;
}

void MainWindow::on_btnAdd_clicked()
{
    on_action_Add_triggered();
}

// Edit
void MainWindow::on_action_Edit_triggered()
{
    if (!checkSelection()) return;
    if (selection.count()>1) {
        QMessageBox::critical(0, tr("Error"), tr("Group editing not impemented"));
        return;
    }
    ContactDialog* d = new ContactDialog(0);
    ContactItem& c = selectedModel->beginEditRow(selection[0]);
    d->setData(c);
    d->exec();
    if (d->result()==QDialog::Accepted) {
        d->getData(c);
        selectedModel->endEditRow(selection[0]);
    }
    delete d;
}

void MainWindow::on_btnEdit_clicked()
{
    on_action_Edit_triggered();
}

void MainWindow::rowDoubleClicked(const QModelIndex &)
{
    on_action_Edit_triggered();
}

// Remove
void MainWindow::on_action_Remove_triggered()
{
    if (!checkSelection()) return;
    if (QMessageBox::question(0, tr("Confirm"), tr("Are You really want to delete selected items?"), tr("Yes"), tr("No"))==0)
        selectedModel->removeAnyRows(selection);
    updateHeaders();
}

void MainWindow::on_btnRemove_clicked()
{
    on_action_Remove_triggered();
}

// Copy
void MainWindow::on_action_Copy_triggered()
{
    if (!checkSelection()) return;
    ContactModel* target = oppositeModel();
    selectedModel->copyRows(selection, target);
    selectedView->clearSelection();
    setButtonsAccess();
    updateHeaders();
}

void MainWindow::on_btnCopy_clicked()
{
    on_action_Copy_triggered();
}

// Move
void MainWindow::on_action_Move_triggered()
{
    if (!checkSelection()) return;
    ContactModel* target = oppositeModel();
    selectedModel->copyRows(selection, target);
    selectedModel->removeAnyRows(selection);
    selectedView->clearSelection();
    setButtonsAccess();
    updateHeaders();
}

void MainWindow::on_btnMove_clicked()
{
    on_action_Move_triggered();
}

// Swap names
void MainWindow::on_action_Swap_names_triggered()
{
    if (!checkSelection()) return;
    selectedModel->swapNames(selection);
    updateHeaders();
}

void MainWindow::on_btnSwapNames_clicked()
{
    on_action_Swap_names_triggered();
}

// Compare two lists
void MainWindow::on_actionCo_mpare_triggered()
{
    // TODO clear both selections before compare
}

void MainWindow::on_btnCompare_clicked()
{
    on_actionCo_mpare_triggered();
}

// Sort List
void MainWindow::on_action_Sort_toggled(bool needSort)
{
    ui->tvLeft->setSortingEnabled(needSort);
    ui->tvRight->setSortingEnabled(needSort);
    updateMode();
}

void MainWindow::on_btnSort_clicked()
{
    ui->action_Sort->toggle();
}

void MainWindow::selectView(QTableView* view)
{
    selectedView = view;
    bool isLeft = selectedView==ui->tvLeft;
    selectedModel = (isLeft ? modLeft : modRight);
    selectedHeader = (isLeft ? ui->lbLeft : ui->lbRight);
}

bool MainWindow::checkSelection()
{
    selection = selectedView->selectionModel()->selectedRows();
    if (selection.count()==0) {
        QMessageBox::critical(0, tr("Error"), tr("Record not selected"));
        return false;
    }
    else return true;
}

void MainWindow::updateListHeader(ContactModel *model, QLabel *header)
{
    QString sChanged = model->changed() ? "*" : "";
    header->setText(sChanged+model->source());
}

void MainWindow::setButtonsAccess()
{
    bool hasSelectedRows = selectedView->selectionModel()->selectedRows().count();
    bool twoPanels = ui->tvRight->isVisible();
    ui->action_Copy->setEnabled(hasSelectedRows && twoPanels);
    ui->action_Move->setEnabled(hasSelectedRows && twoPanels);
    ui->actionCo_mpare->setEnabled(hasSelectedRows && twoPanels);
    ui->action_Edit->setEnabled(hasSelectedRows && (selectedView->selectionModel()->selectedRows().count()==1));
    ui->action_Remove->setEnabled(hasSelectedRows);
    ui->action_Swap_names->setEnabled(hasSelectedRows);
    ui->btnCopy->setEnabled(hasSelectedRows && twoPanels);
    ui->btnMove->setEnabled(hasSelectedRows && twoPanels);
    ui->btnCompare->setEnabled(hasSelectedRows && twoPanels);
    ui->btnEdit->setEnabled(hasSelectedRows && (selectedView->selectionModel()->selectedRows().count()==1));
    ui->btnRemove->setEnabled(hasSelectedRows);
    ui->btnSwapNames->setEnabled(hasSelectedRows);
}

void MainWindow::on_tvLeft_clicked(const QModelIndex&)
{
    setButtonsAccess();
}

void MainWindow::on_tvRight_clicked(const QModelIndex&)
{
    setButtonsAccess();
}

void MainWindow::updateHeaders()
{    
    updateListHeader(modLeft, ui->lbLeft);
    updateListHeader(modRight, ui->lbRight);
    setWindowTitle(selectedModel->source().isEmpty() ?
        tr("Double Contact") :
                       tr("Double Contact - %1").arg(selectedHeader->text()));
}

void MainWindow::updateMode()
{
    QString sm = tr("Mode: ");
    sm += (setDlg->showTwoPanels() ? tr("two panels") : tr("one panel")) + ", ";
    sm += (ui->action_Sort->isChecked() ? tr("sorted") : tr("not sorted")) + ", ";
    // TODO save sorting state in settings and restore it!
    sm += tr("simple editing"); // TODO for manual search, auto compare, duplicate search
    lbMode->setText(sm);
}

ContactModel* MainWindow::oppositeModel()
{
    return (selectedView==ui->tvLeft) ? modRight : modLeft;
}

void MainWindow::askSaveChanges(QCloseEvent *event, ContactModel *model)
{
    if (!model->changed())
        return;
    int res = QMessageBox::question(0, tr("Confirmation"),
            tr("'%1' was changed.\nSave changes?").arg(model->source()),
            QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    switch (res) {
    case QMessageBox::Yes:
        if (model->save())
            event->accept();
        else
            event->ignore();
        break;
    case QMessageBox::No:
        event->accept();
        break;
    case QMessageBox::Cancel:
        event->ignore();
        break;
    }
}

void MainWindow::updateConfig()
{
    modLeft->setVisibleColumns(setDlg->columnNames());
    if (modRight)
        modRight->setVisibleColumns(setDlg->columnNames());
    // TODO lang, surname...
}

void MainWindow::on_action_Other_panel_triggered()
{
    if (selectedView==ui->tvLeft)
        selectView(ui->tvRight);
    else
        selectView(ui->tvLeft);
    selectedView->setFocus();
}

void MainWindow::on_actionSettings_triggered()
{
    setDlg->exec();
    if (setDlg->result()==QDialog::Accepted) {
        setDlg->writeConfig();
        updateConfig();
    }
}

void MainWindow::on_action_Close_triggered()
{
    // TODO ask if model has unsaved changes (after save implementation)
    selectedModel->close();
    updateHeaders();
}
