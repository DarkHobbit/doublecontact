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
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    modLeft = new ContactModel(this, tr("New contact list"));
    ui->tvLeft->setModel(modLeft);
    modRight = new ContactModel(this, tr("New contact list 2"));
    ui->tvRight->setModel(modRight);
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
    ui->action_Two_panels->setChecked(setDlg->showTwoPanels());
    updateHeaders();
    on_action_Two_panels_toggled(ui->action_Two_panels->isChecked());
}

MainWindow::~MainWindow()
{
    delete setDlg;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    askSaveChanges(event, ui->tvLeft);
    askSaveChanges(event, ui->tvRight);
}

void MainWindow::showEvent(QShowEvent *event)
{
    updateConfig();
}

void MainWindow::anyFocusChanged(QWidget *, QWidget *now)
{
    if ((now==ui->tvLeft)||(now==ui->tvRight))
        selectView(dynamic_cast<QTableView*>(now));
    setButtonsAccess();
}

void MainWindow::on_actionE_xit_triggered()
{
    close();
}

void MainWindow::on_action_Two_panels_toggled(bool showTwoPanels)
{
    qDebug() << "on_action_Two_panels_toggled";
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
}

void MainWindow::on_btnExit_clicked()
{
    close();
}

// Open
void MainWindow::on_action_Open_triggered()
{
    QString selectedFilter;
    QString path = QFileDialog::getOpenFileName(0, tr("Open contact file"),
        setDlg->lastPath(), FormatFactory::supportedExtensions().join(";;"), &selectedFilter);
    selectedModel->open(path);
    setDlg->setLastPath(path);
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
void MainWindow::on_action_Sort_triggered()
{
    // TODO
}

void MainWindow::on_btnSort_clicked()
{
    on_action_Sort_triggered();
}

void MainWindow::selectView(QTableView* view)
{
    selectedView = view;
    selectedModel = dynamic_cast<ContactModel*>(selectedView->model());
    selectedHeader = (selectedView==ui->tvLeft ? ui->lbLeft : ui->lbRight);
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
    setButtonsAccess(); // TODO focusChanged not works in some cases
}

void MainWindow::on_tvRight_clicked(const QModelIndex&)
{
    setButtonsAccess(); // TODO focusChanged not works in some cases
}

void MainWindow::updateHeaders()
{    
    updateListHeader(dynamic_cast<ContactModel*>(ui->tvLeft->model()), ui->lbLeft);
    updateListHeader(dynamic_cast<ContactModel*>(ui->tvRight->model()), ui->lbRight);
    setWindowTitle(selectedModel->source().isEmpty() ?
        tr("Double Contact") :
        tr("Double Contact - %1").arg(selectedHeader->text()));
}

ContactModel* MainWindow::oppositeModel()
{
    return dynamic_cast<ContactModel*>
            ((selectedView==ui->tvLeft) ? ui->tvRight->model() : ui->tvLeft->model());
}

void MainWindow::askSaveChanges(QCloseEvent *event, QTableView *view)
{
    ContactModel* model = dynamic_cast<ContactModel*>(view->model());
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
    updateHeaders();
}

void MainWindow::on_actionSettings_triggered()
{
    setDlg->exec();
    if (setDlg->result()==QDialog::Accepted) {
        setDlg->writeConfig();
        updateConfig();
    }
}
