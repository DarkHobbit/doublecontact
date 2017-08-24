/* Double Contact
 *
 * Module: Groups dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QStringList>
#include <QVBoxLayout>
#include "globals.h"
#include "groupdialog.h"
#include "helpers.h"
#include "ui_groupdialog.h"

#define S_GR_EMPTY QObject::tr("Empty group name")
#define S_GR_EXISTS QObject::tr("Group already exists")

GroupDialog::GroupDialog(ContactModel& _model) :
    QDialog(0),
    ui(new Ui::GroupDialog),
    model(_model)
{
    ui->setupUi(this);
    // Fill list
    updateTable();
    ui->twGroups->setItemDelegate(new ReadOnlyTableDelegate(true));
    // Hot keys
    new QShortcut(Qt::Key_Insert, this, SLOT(on_btnAdd_clicked()));
    new QShortcut(Qt::Key_Enter, this, SLOT(on_btnEdit_clicked()));
    new QShortcut(Qt::Key_Return, this, SLOT(on_btnEdit_clicked()));
    connect(ui->twGroups, SIGNAL(doubleClicked (const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));
    new QShortcut(Qt::Key_Delete, this, SLOT(on_btnRemove_clicked()));
}

GroupDialog::~GroupDialog()
{
    delete ui;
}

void GroupDialog::showEvent(QShowEvent *)
{
    ui->twGroups->setColumnWidth(0, 0.7*ui->twGroups->width());
    QStringList hdr;
    hdr << S_GROUP << tr("Contacts", "In group");
    ui->twGroups->setHorizontalHeaderLabels(hdr);
}

void GroupDialog::on_btnAdd_clicked()
{
    QString newName = "";
    if (askGroupName(newName)) {
        model.addGroup(newName);
        updateTable(newName);
    }
}

void GroupDialog::on_btnEdit_clicked()
{
    if (!checkSelection())
        return;
    QString oldName = ui->twGroups->selectedItems()[0]->text();
    QString newName = oldName;
    //Rename
    if (askGroupName(newName)) {
        model.renameGroup(oldName, newName);
        updateTable(newName);
    }
}

void GroupDialog::on_btnRemove_clicked()
{
    if (!checkSelection())
        return;
    if (QMessageBox::question(0, S_CONFIRM, S_REMOVE_CONFIRM,
            QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
    {
        QString name = ui->twGroups->selectedItems()[0]->text();
        model.removeGroup(ui->twGroups->selectedItems()[0]->text());
        // Restore to next/previous item selection
        int oldPos = -1;
        for (int i=0; i<ui->twGroups->rowCount(); i++)
            if (ui->twGroups->item(i, 0)->text()==name) {
                oldPos = i;
                break;
            }
        updateTable();
        if (oldPos<ui->twGroups->rowCount())
            ui->twGroups->selectRow(oldPos);
        else
            ui->twGroups->selectRow(oldPos-1);
    }
}

void GroupDialog::on_btnMerge_clicked()
{
    if (!checkSelection())
        return;
    QString unitedName = ui->twGroups->selectedItems()[0]->text();
    QDialog* d = new QDialog(0);
    d->setWindowTitle(tr("Group merge"));
    QGridLayout* l = new QGridLayout();
    d->setLayout(l);
    QLabel* lbUG = new QLabel(tr("United group"));
    l->addWidget(lbUG, 0, 0);
    QLabel* lbUGN = new QLabel(unitedName);
    lbUGN->setFont(QFont("", -1, QFont::Bold));
    l->addWidget(lbUGN, 0, 1);
    QLabel* lbMG = new QLabel(tr("Merged group"));
    l->addWidget(lbMG, 1, 0);
    QComboBox* cbMGN = new QComboBox();
    l->addWidget(cbMGN, 1, 1);
    QStringList candidates = model.itemList().groupStat().keys();
    candidates.removeOne(unitedName);
    cbMGN->addItems(candidates);
    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), d, SLOT(reject()));
    l->addWidget(bb);
    d->exec();
    if (d->result()==QDialog::Accepted) {
        model.mergeGroups(unitedName, cbMGN->currentText());
        updateTable(unitedName);
    }
    delete d;
}

void GroupDialog::on_btnSplit_clicked()
{
    if (!checkSelection())
        return;
    QString existName = ui->twGroups->selectedItems()[0]->text();
    QDialog* d = new QDialog(0);
    d->setWindowTitle(tr("Group split"));
    QVBoxLayout* l = new QVBoxLayout();
    d->setLayout(l);
    l->addWidget(new QLabel(tr("New group")));
    QLineEdit* edNG = new QLineEdit(existName + " 2");
    l->addWidget(edNG);
    l->addWidget(new QLabel(tr("Contacts for move")));
    QListWidget* lst = new QListWidget();
    lst->addItems(model.itemList().contactsInGroup(existName));
    l->addWidget(lst);
    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), d, SLOT(reject()));
    l->addWidget(bb);
    d->exec();
    if (d->result()==QDialog::Accepted) {
        QString newName = edNG->text();
        // Check on existing name
        if (newName.isEmpty())
            QMessageBox::critical(0, S_ERROR, S_GR_EMPTY);
        else
        if (model.itemList().hasGroup(newName))
            QMessageBox::critical(0, S_ERROR, S_GR_EXISTS);
        else
        if (!lst->selectedItems().isEmpty())
        {
            QList<int> movedIndicesInGroup;
            for (int i=0; i<lst->count(); i++)
                if (lst->item(i)->isSelected())
                    movedIndicesInGroup << i;
            model.splitGroup(existName, newName, movedIndicesInGroup);
            updateTable(existName);
        }
    }
    delete d;
}

void GroupDialog::rowDoubleClicked(const QModelIndex &)
{
    on_btnEdit_clicked();
}

bool GroupDialog::askGroupName(QString &name)
{
    QDialog* d = new QDialog(0);
    d->setWindowTitle(S_GROUP_NAME);
    QVBoxLayout* l = new QVBoxLayout();
    d->setLayout(l);
    QLineEdit* ed = new QLineEdit(name);
    l->addWidget(ed);
    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), d, SLOT(reject()));
    l->addWidget(bb);
    d->exec();
    if (d->result()==QDialog::Accepted) {
        QString oldName = name;
        name = ed->text();
        delete d;
        if (name.isEmpty()) {
            QMessageBox::critical(0, S_ERROR, S_GR_EMPTY);
            return false;
        }
        else
        if (name==oldName)
            // This situation must be checked BEFORE already-exists
            return false;
        else
        if (model.itemList().hasGroup(name)) {
            QMessageBox::critical(0, S_ERROR, S_GR_EXISTS);
            return false;
        }
        else
            return true;
    }
    else {
        delete d;
        return false;
    }
}

void GroupDialog::updateTable(const QString& selectedGroup)
{
    ui->twGroups->setRowCount(0);
    QMap<QString, int> gr = model.itemList().groupStat();
    foreach(const QString& g, gr.keys()) {
        int oldCount = ui->twGroups->rowCount();
        ui->twGroups->setRowCount(oldCount+1);
        ui->twGroups->setItem(oldCount, 0, new QTableWidgetItem(g));
        ui->twGroups->setItem(oldCount, 1, new QTableWidgetItem(QString::number(gr[g])));
    }
    if (selectedGroup.isEmpty())
        ui->twGroups->selectRow(0);
    else {
        for (int i=0; i<ui->twGroups->rowCount(); i++) {
            QString gr = ui->twGroups->item(i, 0)->text();
            if (gr==selectedGroup)
                ui->twGroups->selectRow(i);
        }
    }
}

bool GroupDialog::checkSelection()
{
    if (ui->twGroups->selectedItems().isEmpty()) {
        QMessageBox::critical(0, S_ERROR, S_REC_NOT_SEL);
        return false;
    }
    else
        return true;
}
