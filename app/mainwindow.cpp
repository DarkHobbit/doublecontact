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

#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPalette>
#include <QRadioButton>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QUrl>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "configmanager.h"
#include "aboutdialog.h"
#ifdef WITH_CALLS
#include "callwindow.h"
#endif
#include "contactdialog.h"
#include "comparedialog.h"
#include "csvprofiledialog.h"
#include "groupdialog.h"
#include "helpers.h"
#include "innerfilewindow.h"
#include "logwindow.h"
#ifdef WITH_MESSAGES
#include "messagewindow.h"
#endif
#include "multicontactdialog.h"
#include "settingsdialog.h"
#include "sortdialog.h"
#include "tagremovedialog.h"
#include "formats/common/textreport.h"
#include "formats/iformat.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifndef WITH_MESSAGES
    ui->actionMessages->setVisible(false);
#endif
#ifndef WITH_CALLS
    ui->actionCalls->setVisible(false);
#endif
    // Drag'n'drop support
    ui->tvLeft->setDragEnabled(true);
    ui->tvRight->setDragEnabled(true);
    setAcceptDrops(true); // - for URLs
    ui->tvLeft->setDropIndicatorShown(true);
    ui->tvRight->setDropIndicatorShown(true);
    ui->tvLeft->setAcceptDrops(true); // - for separate vCard items
    ui->tvRight->setAcceptDrops(true);
    ui->tvLeft->setDragDropOverwriteMode(false);
    ui->tvRight->setDragDropOverwriteMode(false);
    ui->tvLeft->installEventFilter(this);
    ui->tvRight->installEventFilter(this);
    // Configuration
    configManager.setDefaults(ui->tvLeft->font().toString(),
        ui->tvLeft->palette().color(QPalette::Base).name(),
        ui->tvLeft->palette().color(QPalette::AlternateBase).name());
    configManager.readConfig(); // After contactColumnHeaders.fill()! Else national UI not works    
    // Models
    modLeft = new ContactModel(this, S_NEW_LIST, recent);
    modRight = new ContactModel(this, S_NEW_LIST + " 2", recent);
    proxyLeft  = new QSortFilterProxyModel(this);
    proxyRight  = new QSortFilterProxyModel(this);
    /*ui->tvLeft->setModel(modLeft);
    ui->tvRight->setModel(modRight);*/
    proxyLeft->setSourceModel(modLeft);
    proxyRight->setSourceModel(modRight);
    proxyLeft->setFilterKeyColumn(-1);
    proxyRight->setFilterKeyColumn(-1);
    proxyLeft->setSortRole(SortStringRole);
    proxyRight->setSortRole(SortStringRole);
    proxyLeft->setFilterCaseSensitivity(Qt::CaseInsensitive); // Driver == driver
    proxyRight->setFilterCaseSensitivity(Qt::CaseInsensitive); // Driver == driver
    ui->tvLeft->setModel(proxyLeft);
    ui->tvRight->setModel(proxyRight);
    connect(modLeft, SIGNAL(requestCSVProfile(CSVFile*)), this, SLOT(onRequestCSVProfile(CSVFile*)), Qt::DirectConnection);
    connect(modRight, SIGNAL(requestCSVProfile(CSVFile*)), this, SLOT(onRequestCSVProfile(CSVFile*)), Qt::DirectConnection);
    ui->tvLeft->horizontalHeader()->setStretchLastSection(true);
    ui->tvRight->horizontalHeader()->setStretchLastSection(true);
    connect(ui->tvLeft->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));
    connect(ui->tvRight->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(onSortIndicatorChanged(int,Qt::SortOrder)));
     // Status bar
    lbCount = new QLabel(0);
    lbMode = new QLabel(0);
    statusBar()->addWidget(lbCount);
    statusBar()->addWidget(lbMode);
    // Settings
    ui->retranslateUi(this);
    // Track selected view
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(anyFocusChanged(QWidget*,QWidget*)));
    setSelectionModelEvents();
    connect(ui->tvLeft, SIGNAL(doubleClicked (const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));
    connect(ui->tvRight, SIGNAL(doubleClicked (const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));
    selectView(ui->tvLeft);
    selection = selectedView->selectionModel()->selectedRows();
    recent.read();
    // Context menus
    ui->actionSeparator1->setSeparator(true);
    ui->actionSeparator2->setSeparator(true);
    ui->actionSeparator3->setSeparator(true);
    ui->actionSeparator4->setSeparator(true);
    buildContextMenu(ui->tvLeft);
    buildContextMenu(ui->tvRight);
    // Test data
    if (gd.debugDataMode)
        modLeft->testList();
    // File command-line data
    else if (gd.startupFiles.count()>0 && !gd.quietMode)
    {
        open(modLeft, gd.startupFiles[0], ftAuto);
        if (gd.startupFiles.count()>1) {
            ui->action_Two_panels->setChecked(true);
            open(modRight, gd.startupFiles[1], ftAuto);
        }
    }
    // Previous session file data
    else if (!qApp->arguments().contains("-q")
             && gd.openLastFilesAtStartup
             && QFile(configManager.lastContactFile()).exists()
             && (configManager.lastContactFile()!=ConfigManager::defaultDocDir())) // against falseerrormessage at first run
        open(selectedModel, configManager.lastContactFile(), ftAuto);
    // Show data
    ui->action_Two_panels->setChecked(configManager.showTwoPanels());
    readTableSortConfig(ui->tvLeft, false);
    readTableSortConfig(ui->tvRight, false);
    ui->action_Sort->setChecked(ui->tvLeft->isSortingEnabled());
    updateHeaders();
    updateModeStatus();
    updateRecent();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (askSaveChanges(modLeft)
    && askSaveChanges(modRight))
        event->accept();
    else
        event->ignore();
}

void MainWindow::showEvent(QShowEvent*)
{
    on_action_Two_panels_toggled(ui->action_Two_panels->isChecked());
    updateConfig();
    if (modLeft->rowCount()>0)
        ui->tvLeft->selectRow(0);
    if (modRight->rowCount()>0)
        ui->tvRight->selectRow(0);
}

// Drag'n'drop support
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::DragEnter) {
        QDragEnterEvent* e = static_cast<QDragEnterEvent*>(event);
        e->acceptProposedAction();
        return true;
    }
    else if (event->type() == QEvent::Drop) {
        bool isLeftSource = (obj == ui->tvLeft);
        QDropEvent* e = static_cast<QDropEvent*>(event);
        // How many URLs got?
        if (!e->mimeData()->hasUrls()) {
            return false;
        }
        int urlCount = e->mimeData()->urls().count();
        if (urlCount>2) {
            QMessageBox::critical(0, S_ERROR,
                tr("Only one of two files can be dropped to program window"));
            return false;
        }
        // Open file(s)
        ContactModel* primaryModel = isLeftSource ? modLeft : modRight;
        QString filePath = e->mimeData()->urls()[0].toLocalFile();
        if (askSaveChanges(primaryModel)) {
            open(primaryModel, filePath, QFileInfo(filePath).isDir() ? ftDirectory : ftFile);
            if (urlCount==2) {
                // Add second panel if missing
                if (!ui->tvRight->isVisible())
                    ui->action_Two_panels->setChecked(true); // ???
                filePath = e->mimeData()->urls()[1].toLocalFile();
                ContactModel* secondaryModel = isLeftSource ? modRight : modLeft;
                if (askSaveChanges(secondaryModel))
                    open(secondaryModel, filePath, QFileInfo(filePath).isDir() ? ftDirectory : ftFile);
            }
        }
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
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
    // TODO here switch off quickview or dup search if need activate second panel
    int splitSize = ui->splitter->width();
    if (showTwoPanels) {
        ui->tvRight->show();
        ui->lbRight->show();
        ui->lbFilterRight->show();
        ui->leFilterRight->show();
        ui->splitter->setSizes(QList<int>() << splitSize/2 << splitSize/2);
    }
    else {
        ui->tvRight->hide();
        ui->lbRight->hide();
        ui->lbFilterRight->hide();
        ui->leFilterRight->hide();
        ui->splitter->setSizes(QList<int>() << splitSize);
    }
    selectView(ui->tvLeft);
    ui->tvLeft->setFocus();
    setButtonsAccess();
    configManager.setShowTwoPanels(showTwoPanels);
    updateModeStatus();
}

void MainWindow::on_action_Quick_view_toggled(bool showQuickView)
{
    selectedModel->setViewMode(
        showQuickView ? ContactModel::StandardWithQuickView : ContactModel::Standard,
        oppositeModel());
    QStackedWidget *prevSw = (isLeftPanelActive ? ui->swLeft : ui->swRight),
                   *newSw  = (isLeftPanelActive ? ui->swRight : ui->swLeft);
    prevSw->setCurrentIndex(0);
    newSw->setCurrentIndex(0);
    int splitSize = ui->splitter->width();
    if (showQuickView) {
        if (ui->splitter->sizes().last()==0)
            ui->splitter->setSizes(QList<int>() << splitSize/2 << splitSize/2);
        newSw->setCurrentIndex(1);
        selectionChanged();
    }
    else {
        if (!ui->tvRight->isVisible())
            ui->splitter->setSizes(QList<int>() << splitSize);
    }
}

void MainWindow::on_btnExit_clicked()
{
    close();
}

// Open
void MainWindow::on_action_OpenFile_triggered()
{
    if (!askSaveChanges(selectedModel))
        return;
    QString selectedFilter;
    QString path = QFileDialog::getOpenFileName(0, tr("Open contact file"),
        configManager.lastContactFile(),
        FormatFactory::supportedFilters(QIODevice::ReadOnly, false).join(";;"),
        &selectedFilter);
    if (!path.isEmpty()) {
        open(selectedModel, path, ftFile);
        configManager.setLastContactFile(path);
        updateHeaders();
        updateRecent();
    }
}

void MainWindow::on_action_OpenDir_triggered()
{
    if (!askSaveChanges(selectedModel))
        return;
    QString path = QFileDialog::getExistingDirectory(this,
        tr("Open VCF Directory"), configManager.lastContactFile(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        open(selectedModel, path, ftDirectory);
        configManager.setLastContactFile(path);
        updateHeaders();
        updateRecent();
    }
}

// Save
void MainWindow::on_action_Save_triggered()
{
    if (selectedModel->sourceType()==ftNew)
        on_action_SaveAsFile_triggered();
    else
        saveAs(selectedModel, selectedModel->source(), selectedModel->sourceType());
    updateHeaders();
}

// Save as
void MainWindow::on_action_SaveAsFile_triggered()
{
    QString selectedFilter;
    QString path = QFileDialog::getSaveFileName(0, tr("Save contact file"),
        configManager.lastContactFile(),
        FormatFactory::supportedFilters(QIODevice::WriteOnly, false).join(";;"),
        &selectedFilter);
    if (!path.isEmpty()) {
        selectedModel->itemList().originalProfile.clear(); // force ask profile for new file
        saveAs(selectedModel, path, ftFile);
        configManager.setLastContactFile(path);
        updateHeaders();
        updateRecent();
    }
}

void MainWindow::on_action_SaveAsDir_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this,
        tr("Save VCF Directory"), configManager.lastContactFile(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        QDir d(path);
        if (d.exists()) {
            QStringList entries = QDir(path).entryList();
            entries.removeOne(".");
            entries.removeOne("..");
            if (!entries.isEmpty())
                if (QMessageBox::question(0, S_CONFIRM, tr("Directory exists. Are You really want rewrite it?"),
                   QMessageBox::Yes, QMessageBox::No)!=QMessageBox::Yes)
                    return;
        }
        saveAs(selectedModel, path, ftDirectory);
        configManager.setLastContactFile(path);
        updateHeaders();
    }
}

// Add
void MainWindow::on_action_Add_triggered()
{
    ContactDialog* d = new ContactDialog(0);
    d->clearData(selectedModel->itemList());
    d->exec();
    if (d->result()==QDialog::Accepted) {
        ContactItem c;
        d->getData(c, selectedModel->itemList());
        selectedModel->addRow(c);
        updateViewMode();
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
    if (!checkSelection(true))
        return;
    if (selection.count()==1) // Ordinary edition
    {
        ContactDialog* d = new ContactDialog(0);
        ContactItem& c = selectedModel->beginEditRow(selection[0]);
        d->setData(c, selectedModel->itemList());
        d->exec();
        if (d->result()==QDialog::Accepted) {
            d->getData(c, selectedModel->itemList());
            selectedModel->endEditRow(selection[0]);
            updateViewMode();
        }
        delete d;
    }
    else
    if (QMessageBox::question(0, S_CONFIRM, tr("Are You really want to edit more than one record?\nOnly some fields can this edited in this mode"),
                QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
    { // Multi-record edition
        MultiContactDialog* d = new MultiContactDialog(0);
        for (int i=0; i<selection.count(); i++)
            d->setData(selectedModel->beginEditRow(selection[i]), i==0);
        d->exec();
        if (d->result()==QDialog::Accepted) {
            for (int i=0; i<selection.count(); i++) {
                d->getData(selectedModel->beginEditRow(selection[i]));
                selectedModel->endEditRow(selection[i]);
            }
            updateViewMode();
        }
        delete d;
    }
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
    if (QMessageBox::question(0, S_CONFIRM, S_REMOVE_CONFIRM,
            QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
        selectedModel->removeAnyRows(selection);
    updateViewMode();
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
    updateViewMode();
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
    updateViewMode();
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
    updateViewMode();
    updateHeaders();
}

// Split names
void MainWindow::on_actionS_plit_names_triggered()
{
    if (!checkSelection()) return;
    selectedModel->splitNames(selection);
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_btnSwapNames_clicked()
{
    on_action_Swap_names_triggered();
}

// Compare two lists
void MainWindow::on_actionCo_mpare_triggered()
{
    // Clear both selections before compare
    ui->tvLeft->selectionModel()->clearSelection();
    ui->tvRight->selectionModel()->clearSelection();
    // Compare on
    bool notInCompareMode = selectedModel->viewMode()!=ContactModel::CompareMain
        && selectedModel->viewMode()!=ContactModel::CompareOpposite;
    if (notInCompareMode) {
        // TODO here switch off quick view and/or dup search (m.b.not only here)
        // TODO if opposite panel unsaved and hidden before program closing, show it before ask (not here)
        if (!ui->tvRight->isVisible() || oppositeModel()->rowCount()==0 || selectedModel->rowCount()==0) {
            QMessageBox::critical(0, S_ERROR,
                tr("Compare mode requires show two panels and load contact lists in both panels"));
            return;
        }
        selectedModel->setViewMode(ContactModel::CompareMain, oppositeModel());
    }
    // Compare off
    else
        selectedModel->setViewMode(ContactModel::Standard, oppositeModel());
    updateModeStatus();
}

void MainWindow::on_btnCompare_clicked()
{
    on_actionCo_mpare_triggered();
}

// Sort List
void MainWindow::on_action_Sort_toggled(bool needSort)
{
    readTableSortConfig(selectedView, true, needSort);
    writeTableSortConfig(selectedView);
    updateModeStatus();
}

void MainWindow::onSortIndicatorChanged(int, Qt::SortOrder)
{
    QHeaderView* header = dynamic_cast<QHeaderView*>(sender());
    if (header)
        writeTableSortConfig(header);
}

void MainWindow::on_btnSort_clicked()
{
    ui->action_Sort->toggle();
}

void MainWindow::selectView(QTableView* view)
{
    selectedView = view;
    isLeftPanelActive = selectedView==ui->tvLeft;
    selectedModel = (isLeftPanelActive ? modLeft : modRight);
    selectedHeader = (isLeftPanelActive ? ui->lbLeft : ui->lbRight);
    selectedQVPanel = (isLeftPanelActive ? ui->swRigthPageDetails : ui->swLeftPageDetails); // Vice versa!
}

bool MainWindow::checkSelection(bool errorIfNoSelected, bool onlyOneRowAllowed)
{
    QModelIndexList proxySelection = selectedView->selectionModel()->selectedRows();
    if (proxySelection.count()==0) {
        if (errorIfNoSelected)
            QMessageBox::critical(0, S_ERROR, S_REC_NOT_SEL);
        return false;
    }
    if (onlyOneRowAllowed && (proxySelection.count()>1)) {
        QMessageBox::critical(0, S_ERROR, S_ONLY_ONE_REC);
        return false;
    }
    // If proxy models works...
    QSortFilterProxyModel* selectedProxy = (selectedView==ui->tvLeft) ? proxyLeft : proxyRight;
    selection.clear();
    foreach(QModelIndex index, proxySelection)
        selection << selectedProxy->mapToSource(index);
    return true;
}

void MainWindow::updateListHeader(ContactModel *model, QLabel *header)
{
    QString sChanged = model->changed() ? "*" : "";
    QString src = model->source();
    if (src.contains(QDir::separator()))
        src = src.mid(src.lastIndexOf(QDir::separator())+1);
    if (model->sourceType()==ftDirectory)
        src += tr(" (directory)");
    header->setText(sChanged+src);
    // Highlight selected panel header
    QPalette pal = header->palette();
    pal.setColor(QPalette::WindowText, pal.color(QPalette::HighlightedText));
    pal.setColor(QPalette::Window, pal.color(QPalette::Highlight));
    header->setAutoFillBackground(true);
    header->setPalette(pal);
    oppositeHeader()->setPalette(QPalette());
}

void MainWindow::setButtonsAccess()
{
    bool hasSelectedRows = selectedView->selectionModel()->selectedRows().count();
    bool twoPanels = ui->tvRight->isVisible();
    ui->action_Copy->setEnabled(hasSelectedRows && twoPanels);
    ui->action_Move->setEnabled(hasSelectedRows && twoPanels);
    ui->actionCo_mpare->setEnabled(twoPanels);
    ui->action_Edit->setEnabled(hasSelectedRows);
    ui->action_Remove->setEnabled(hasSelectedRows);
    ui->action_Swap_names->setEnabled(hasSelectedRows);
    ui->btnCopy->setEnabled(hasSelectedRows && twoPanels);
    ui->btnMove->setEnabled(hasSelectedRows && twoPanels);
    ui->btnCompare->setEnabled(hasSelectedRows && twoPanels);
    ui->btnEdit->setEnabled(hasSelectedRows);
    ui->btnRemove->setEnabled(hasSelectedRows);
    ui->btnSwapNames->setEnabled(hasSelectedRows);
    ui->action_Sort->setChecked(selectedView->isSortingEnabled());
#ifdef WITH_MESSAGES
    ui->actionMessages->setEnabled(selectedModel->itemList().hasMessages());
#endif
#ifdef WITH_CALLS
    ui->actionCalls->setEnabled(selectedModel->itemList().hasCalls());
#endif
    ui->action_Files->setEnabled(selectedModel->itemList().hasFiles());
}

void MainWindow::selectionChanged()
{
    static bool lockSelection = false;
    // For compare mode, select pair item(s)
    ContactModel::ContactViewMode viewMode = selectedModel->viewMode();
    if (!lockSelection) {
        if (viewMode==ContactModel::CompareMain || viewMode==ContactModel::CompareOpposite) {
            if (!checkSelection(false)) return;
            QTableView* oppView = (selectedView==ui->tvLeft) ? ui->tvRight : ui->tvLeft;
            QSortFilterProxyModel* oppProxy = dynamic_cast<QSortFilterProxyModel*>(oppView->model());
            oppView->selectionModel()->clearSelection();
            foreach (const QModelIndex& index, selection) {
                ContactItem& item = selectedModel->itemList()[index.row()];
                if (item.pairItem) {
                    QModelIndex oppIndexFirst = oppProxy->mapFromSource(
                        oppositeModel()->index(item.pairIndex, 0));
                    QModelIndex oppIndexLast = oppProxy->mapFromSource(
                        oppositeModel()->index(item.pairIndex, selectedModel->columnCount()-1));
                    lockSelection = true;
                    oppView->selectionModel()->select(QItemSelection(oppIndexFirst, oppIndexLast), QItemSelectionModel::Select);
                    lockSelection = false;
                }
            }
        }
        else if (viewMode==ContactModel::StandardWithQuickView)
            updateQuickView();
    }
    setButtonsAccess();
}

void MainWindow::recentItemClicked()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if (action) {
        QString path = action->text();
        QFileInfo fi(path);
        if (fi.exists()) {
            if (!askSaveChanges(selectedModel))
                return;
            open(selectedModel, path, ftAuto);
            configManager.setLastContactFile(path);
        }
        updateRecent();
        updateHeaders();
    }
}

void MainWindow::clearRecentClicked()
{
    recent.clear();
    recent.write();
    updateRecent();
}

void MainWindow::onRequestCSVProfile(CSVFile *format)
{
    CSVProfileDialog* d = new CSVProfileDialog(0);
    d->selectProfile(format);
    delete d;
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
    // List headers
    updateListHeader(modLeft, ui->lbLeft);
    updateListHeader(modRight, ui->lbRight);
    // Window
    setWindowTitle(selectedModel->source().isEmpty() ?
        QString("Double Contact") :
                       QString("DoubleContact - %1").arg(selectedHeader->text()));
    // Status bar
    lbCount->setText(tr("Records: %1").arg(selectedModel->rowCount()));
}

void MainWindow::updateModeStatus()
{
    QString sm = SS_MODE;
    sm += (configManager.showTwoPanels() ? tr("two panels") : tr("one panel")) + ", ";
    sm += (ui->action_Sort->isChecked() ? SS_SORT_ON : SS_SORT_OFF) + ", ";
    switch (modLeft->viewMode()) {
    case ContactModel::Standard:
        sm += tr("simple editing");
        break;
    case ContactModel::CompareMain:
    case ContactModel::CompareOpposite:
        sm += tr("compare");
        break;
    default:
        // TODO duplicate search
        break;
    }
    lbMode->setText(sm);
}

void MainWindow::updateViewMode()
{
    selectedModel->setViewMode(selectedModel->viewMode(), oppositeModel());
    if (gd.resizeTableRowsToContents)
        selectedView->resizeRowsToContents();
    // TODO when switch on quick view on one panel, switch off it on other panel, if present
    // Learn, here called updateViewMode() and don't forget about on_actionCo_mpare_triggered()
}

QLabel* MainWindow::createBoldTopLabel(const QString& text)
{
    QLabel* lb = new QLabel(QString("<b>%1</b>").arg(text));
    lb->setAlignment(Qt::AlignTop);
    return lb;
}

void MainWindow::updateQuickView()
{
    // TODO check QV logic on Tab and Ctrl+U
    // Delete old quickview
    QGridLayout* l = dynamic_cast<QGridLayout*>(selectedQVPanel->layout());
    foreach (QObject* ch, selectedQVPanel->children())
        if (dynamic_cast<QWidget*>(ch))
            delete ch;
    delete l;
    // Generate new quickview
    l = new QGridLayout();
    l->setColumnStretch(1, 2);
    if (!checkSelection(false) || selection.count()!=1)
        return;
    const QModelIndex& index = selection.first();
    ContactItem& item = selectedModel->itemList()[index.row()];
    TextReport::RepItems out;
    TextReport::exportRecord(item, out, "\n");
    l->addWidget(createBoldTopLabel(out.title), 0, 1);
    for (int i=0; i<out.count(); i++) {
        l->addWidget(createBoldTopLabel(out[i].first), i+1, 0);
        l->addWidget(new QLabel(out[i].second), i+1, 1);
    }
    int lastCol = out.count()+1;
    // Media if available
    if (!item.photo.isEmpty() && item.photo.pType!="URL") {
        l->addWidget(createBoldTopLabel(S_PHOTO), lastCol, 0);
        QLabel* lbPhoto = new QLabel();
        lbPhoto->setMaximumHeight(256);
        lbPhoto->setMaximumHeight(192);
        showPhoto(item.photo, lbPhoto);
        l->addWidget(lbPhoto, lastCol, 1);
        lastCol++;
    }
    // Page down
    QSpacerItem* vSp = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    l->addItem(vSp, lastCol, 0);
    selectedQVPanel->setLayout(l);
}

void MainWindow::setSelectionModelEvents()
{
    connect(ui->tvLeft->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(setButtonsAccess()));
    connect(ui->tvRight->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(setButtonsAccess()));
    connect(ui->tvLeft->selectionModel(), SIGNAL(selectionChanged( const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChanged()));
    connect(ui->tvRight->selectionModel(), SIGNAL(selectionChanged( const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChanged()));
}

ContactModel* MainWindow::oppositeModel()
{
    return (selectedView==ui->tvLeft) ? modRight : modLeft;
}

QLabel *MainWindow::oppositeHeader()
{
    return (selectedHeader==ui->lbLeft) ? ui->lbRight : ui->lbLeft;
}

bool MainWindow::askSaveChanges(ContactModel *model)
{
    bool res = false;
    if (!model->changed()) {
        res = true;
    }
    else {
        int btn = QMessageBox::question(0, S_CONFIRM,
             tr("'%1' was changed.\nSave changes?").arg(model->source()),
             QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        switch (btn) {
        case QMessageBox::Yes:
            on_action_Save_triggered();
            res = model->sourceType()!=ftNew;
            break;
        case QMessageBox::No:
            res = true;
            break;
        case QMessageBox::Cancel:
            res = false;
        }
    }
    if (res && (model->sourceType()!=ftNew) && (!model->source().isEmpty()))
        updateRecent();
    return res;
}

bool MainWindow::open(ContactModel *model, const QString &path, FormatType fType)
{
    QStringList errors;
    QString fatalError;
    bool res = model->open(path, fType, errors, fatalError);
    showIOErrors(path, model->rowCount(), errors, fatalError);
    if (model->rowCount()>0)
        selectedView->selectRow(0);
    return res;
}

bool MainWindow::saveAs(ContactModel *model, const QString &path, FormatType fType)
{
    QStringList errors;
    QString fatalError;
    //===>
    if (path.endsWith(".nbf", Qt::CaseInsensitive))
        QMessageBox::warning(0, "WARNING", "NBF save not tested. Your phone may be corrupted.");
    //===>
    // Save
    bool res = model->saveAs(path, fType, errors, fatalError);
    showIOErrors(path, model->rowCount(), errors, fatalError);
    return res;
}

void MainWindow::showIOErrors(const QString &path, int count, const QStringList &errors, const QString &fatalError)
{
    if (!errors.isEmpty()) {
        LogWindow* w = new LogWindow(0);
        w->setData(path, count, errors);
        w->exec();
        delete w;
    }
    if (!fatalError.isEmpty())
        QMessageBox::critical(0, S_ERROR, fatalError);
}

// Manage immediately applied but settings window managed options
void MainWindow::updateConfig()
{
    // Table(s) visible columns
    modLeft->updateVisibleColumns();
    if (modRight)
        modRight->updateVisibleColumns();
    // Table(s) general config (must be after updateVisibleColumns(), because resizeRowsToContents())
    updateTableConfig(ui->tvLeft);
    updateTableConfig(ui->tvRight);
}

void MainWindow::updateRecent()
{
    ui->menuRecent->clear();
    foreach (const QString& s, recent)
        ui->menuRecent->addAction(s, this, SLOT(recentItemClicked()));
    if (!recent.isEmpty()) {
        ui->menuRecent->addSeparator();
        ui->menuRecent->addAction(S_CLEAR_RECENT, this, SLOT(clearRecentClicked()));
    }
    ui->menuRecent->setEnabled(recent.count()>0);
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
    SettingsDialog* setDlg = new SettingsDialog(0);
    setDlg->setData();
    setDlg->exec();
    if (setDlg->result()==QDialog::Accepted) {
        setDlg->getData();
        configManager.writeConfig();
        updateConfig();
        // Language
        if (setDlg->langChanged())
            QMessageBox::information(0, S_INFORM, tr("Restart program to apply language change"));
    }
    delete setDlg;
}

void MainWindow::on_action_Close_triggered()
{
    if (!askSaveChanges(selectedModel))
        return;
    selectedModel->close();
    updateHeaders();
    updateRecent();
}

void MainWindow::on_leFilterLeft_textChanged(const QString &arg1)
{
    if (arg1.isEmpty())
        proxyLeft->setFilterWildcard("");
    else
        proxyLeft->setFilterWildcard(arg1);
}

void MainWindow::on_leFilterRight_textChanged(const QString &arg1)
{
    if (arg1.isEmpty())
        proxyRight->setFilterWildcard("");
    else
        proxyRight->setFilterWildcard(arg1);
}

void MainWindow::on_action_Filter_triggered()
{
    if (selectedView==ui->tvLeft)
        ui->leFilterLeft->setFocus();
    else
        ui->leFilterRight->setFocus();
}

void MainWindow::on_actionCompare_Result_triggered()
{
    // Check two panels and compare mode
    if ((!ui->tvRight->isVisible())
            || (selectedModel->viewMode()!=ContactModel::CompareMain
            && selectedModel->viewMode()!=ContactModel::CompareOpposite)) {
        QMessageBox::critical(0, S_ERROR, tr("Two panels and compare mode needed for this operation"));
        return;
    }
    // Remember panel and prepare op
    bool wasRight = (selectedModel==modRight);
    if (wasRight)
            on_action_Other_panel_triggered(); // to left
    if (!checkSelection(true, true)) return;
    QModelIndex leftSelection = selection[0];
    ContactItem& left = modLeft->beginEditRow(leftSelection);
    on_action_Other_panel_triggered(); // to right
    if (!checkSelection(true, true)) return;
    ContactItem& right = modRight->beginEditRow(selection[0]);
    // Show results
    CompareDialog* d = new CompareDialog(0);
    d->setHeaders(tr("Left item"), tr("Right item"));
    d->setData(left, right);
    d->exec();
    if (d->result()==QDialog::Accepted)
        d->getData(left, right);
    modLeft->endEditRow(leftSelection);
    modRight->endEditRow(selection[0]);
    delete d;
    // Restore panel
    if (!wasRight)
        on_action_Other_panel_triggered(); // to left
    updateViewMode();
}

void MainWindow::on_action_Drop_slashes_triggered()
{
    if (!checkSelection()) return;
    selectedModel->dropSlashes(selection);
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_action_Join_triggered()
{
    if (!checkSelection()) return;
    // Operation required strongly 2 records
    if (selection.count()!=2) {
        QMessageBox::critical(0, S_ERROR, tr("Strongly two records on current panel must be selected for this operation"));
        return;
    }
    ContactItem& i1 = selectedModel->beginEditRow(selection[0]);
    ContactItem& i2 = selectedModel->beginEditRow(selection[1]);
    CompareDialog* d = new CompareDialog(0);
    d->setHeaders(tr("Item 1"), tr("Item 2"));
    d->setData(i1, i2);
    d->exec();
    if (d->result()==QDialog::Accepted) {
        d->getData(i1, i2);
        selectedModel->endEditRow(selection[0]);
        selectedModel->endEditRow(selection[1]);
        updateViewMode();
        updateHeaders();
    }
    delete d;
}

void MainWindow::on_actionSp_lit_triggered()
{
    if (!checkSelection()) return;
    selectedModel->splitNumbers(selection);
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_action_Generate_full_name_triggered()
{
    if (!checkSelection()) return;
    selectedModel->generateFullNames(selection);
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_actionDrop_full_name_triggered()
{
    if (!checkSelection()) return;
    selectedModel->dropFullNames(selection);
    updateViewMode();
    updateHeaders();
}


void MainWindow::on_actionIntl_phone_prefix_triggered()
{
    if (!checkSelection()) return;
    QDialog* d = new QDialog(0);
    d->setWindowTitle(S_COUNTRY_RULE_SELECT);
    QVBoxLayout* l = new QVBoxLayout();
    d->setLayout(l);
    QComboBox* cbCountryRule = new QComboBox();
    cbCountryRule->addItems(Phone::availableCountryRules());
    if (gd.defaultCountryRule>=0 && gd.defaultCountryRule<COUNTRY_RULES_COUNT)
        cbCountryRule->setCurrentIndex(gd.defaultCountryRule);
    l->addWidget(cbCountryRule);
    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), d, SLOT(reject()));
    l->addWidget(bb);
    d->setMinimumWidth(400); // Window title is too long
    d->exec();
    if (d->result()==QDialog::Accepted) {
        selectedModel->intlPhonePrefix(selection, cbCountryRule->currentIndex());
        updateViewMode();
        updateHeaders();
    }
    delete d;
}

void MainWindow::on_actionS_wap_Panels_triggered()
{
    if (!ui->tvRight->isVisible()) {
        QMessageBox::critical(0, S_ERROR,
            tr("Operation requires show two panels"));
        return;
    }
    ContactModel* bufModel = modLeft;
    modLeft = modRight;
    modRight = bufModel;
    QSortFilterProxyModel* bufProxy = proxyLeft;
    proxyLeft = proxyRight;
    proxyRight = bufProxy;
    ui->tvLeft->setModel(proxyLeft);
    ui->tvRight->setModel(proxyRight);
    updateHeaders();
    setSelectionModelEvents();
}


void MainWindow::on_action_About_triggered()
{
    AboutDialog* d = new AboutDialog(0);
    d->exec();
    delete d;
}

void MainWindow::on_actionReverse_full_name_triggered()
{
    if (!checkSelection()) return;
    selectedModel->reverseFullNames(selection);
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_actionS_tatistics_triggered()
{
    QMessageBox::information(0, tr("Statitics"),
        selectedModel->itemList().statistics());
}

void MainWindow::on_actionRe_port_triggered()
{
    QString selectedFilter;
    QString path = QFileDialog::getSaveFileName(0, tr("Report file"),
        ConfigManager::defaultDocDir() + QDir::separator() + "report.html",
        FormatFactory::supportedFilters(QIODevice::WriteOnly, true).join(";;"),
        &selectedFilter);
    if (!path.isEmpty()) {
        QStringList errors;
        QString fatalError;
        FormatFactory factory;
        IFormat* format = factory.createObject(path);
        if (!format)
            fatalError = factory.error;
        else {
            format->exportRecords(path, selectedModel->itemList());
            fatalError = format->fatalError();
            errors = format->errors();
            delete format;
        }
        showIOErrors(path, selectedModel->rowCount(), errors, fatalError);
    }
}

void MainWindow::on_action_Hard_sort_triggered()
{
    // Ask for sort fields order
    SortDialog* d = new SortDialog(0);    
    d->setData(configManager.hardSortType());
    d->exec();
    if (d->result()==QDialog::Accepted) {
        // Sort
        configManager.setHardSortType(d->getData());
        selectedModel->hardSort(d->getData());
        // Warning if sort view switched on
        if (ui->action_Sort->isChecked())
            if (QMessageBox::question(0, S_CONFIRM, S_SORTMASK_CONFIRM,
                    QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
                ui->action_Sort->toggle();
    }
    delete d;
    // Refresh view
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_action_Groups_triggered()
{
    GroupDialog* d = new GroupDialog(*selectedModel);
    d->exec();
    delete d;
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_action_About_Qt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::buildContextMenu(QTableView *view)
{
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
    view->addAction(ui->action_Add);
    view->addAction(ui->action_Edit);
    view->addAction(ui->action_Remove);

    view->addAction(ui->actionSeparator1);
    view->addAction(ui->action_Copy);
    view->addAction(ui->action_Move);
    view->addAction(ui->actionCopy_text);

    view->addAction(ui->actionSeparator2);
    view->addAction(ui->action_Join);
    view->addAction(ui->actionSp_lit);

    view->addAction(ui->actionSeparator3);
    view->addAction(ui->action_Swap_names);
    view->addAction(ui->actionCompare_Result);
    view->addAction(ui->actionS_plit_names);
    view->addAction(ui->action_Drop_slashes);
    view->addAction(ui->actionIntl_phone_prefix);
    view->addAction(ui->actionFormat_phone_numbers);

    view->addAction(ui->actionSeparator4);
    view->addAction(ui->action_Generate_full_name);
    view->addAction(ui->actionDrop_full_name);
    view->addAction(ui->actionReverse_full_name);
}

void MainWindow::on_actionFormat_phone_numbers_triggered()
{
    if (!checkSelection()) return;
    QDialog* d = new QDialog(0);
    d->setWindowTitle(S_COUNTRY_PHONE_TEMPLATE);
    QVBoxLayout* l = new QVBoxLayout();
    d->setLayout(l);
    QComboBox* cbPhoneTemplate = new QComboBox();
    cbPhoneTemplate->addItem("+N (NNN) NNN-NN-NN");
    cbPhoneTemplate->addItem("+N NNN NNN-NN-NN");
    cbPhoneTemplate->addItem("+N-NNN-NNN-NN-NN");
    cbPhoneTemplate->addItem("+NNNNNNNNNNN");
    cbPhoneTemplate->setEditable(true);
    // TODO set combobox editable
    l->addWidget(cbPhoneTemplate);
    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), d, SLOT(reject()));
    l->addWidget(bb);
    d->setMinimumWidth(400); // Window title is too long
    d->exec();
    if (d->result()==QDialog::Accepted) {
        selectedModel->formatPhones(selection, cbPhoneTemplate->currentText());
        updateViewMode();
        updateHeaders();
    }
    delete d;
}

void MainWindow::on_actionCopy_text_triggered()
{
    QModelIndex ind = selectedView->selectionModel()->currentIndex();
    QSortFilterProxyModel* selectedProxy = (selectedView==ui->tvLeft) ? proxyLeft : proxyRight;
    ind = selectedProxy->mapToSource(ind);
    QString text = selectedModel->data(ind, Qt::DisplayRole).toString();
    qApp->clipboard()->setText(text);
}

void MainWindow::on_actionJoin_names_triggered()
{
    if (!checkSelection()) return;
    selectedModel->joinNames(selection);
    updateViewMode();
    updateHeaders();
}

void MainWindow::on_actionParse_full_name_triggered()
{
    if (!checkSelection()) return;
    selectedModel->parseFullName(selection);
    updateViewMode();
    updateHeaders();
}

#ifdef WITH_MESSAGES
void MainWindow::on_actionMessages_triggered()
{
    MessageWindow* w = new MessageWindow(&selectedModel->itemList());
    w->exec();
    delete w;
}
#endif

#ifdef WITH_CALLS
void MainWindow::on_actionCalls_triggered()
{
    CallWindow* w = new CallWindow(&selectedModel->itemList());
    w->exec();
    delete w;
}
#endif

void MainWindow::on_actionMass_tags_remove_triggered()
{
    TagRemoveDialog* d = new TagRemoveDialog(0);
    if (!d->setData(selectedModel->itemList()))
        return;
    d->exec();
    // Remove
    if (d->result()==QDialog::Accepted) {
        selectedModel->massTagRemove(d->getData());
        updateViewMode();
        updateHeaders();
    }
    delete d;
}


void MainWindow::on_action_Files_triggered()
{
    InnerFileWindow* w = new InnerFileWindow(&selectedModel->itemList());
    w->exec();
    delete w;
    /* fgh atr
    QMessageBox::information(0, "Dbg", QString("c %1 s %2")
        .arg(selectedModel->itemList().extra.files.count())
        .arg(selectedModel->itemList().extra.files.totalSize()));
*/
}

