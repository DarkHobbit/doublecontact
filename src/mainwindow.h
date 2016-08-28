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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QModelIndexList>
#include <QTableView>

#include "contactmodel.h"
#include "contactsorterfilter.h"
#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent*);
private slots:
    void on_action_Other_panel_triggered();
    void setButtonsAccess();
    void selectionChanged();
    void on_actionCo_mpare_triggered();
    void on_btnCompare_clicked();
    void anyFocusChanged (QWidget*, QWidget* now);
    void on_actionE_xit_triggered();
    void on_action_Two_panels_toggled(bool showTwoPanels);
    void on_btnExit_clicked();
    void on_action_Add_triggered();
    void on_btnAdd_clicked();
    void on_action_Edit_triggered();
    void on_btnEdit_clicked();
    void on_action_Remove_triggered();
    void on_btnRemove_clicked();
    void on_action_Copy_triggered();
    void on_btnCopy_clicked();
    void on_action_Move_triggered();
    void on_btnMove_clicked();
    void on_action_Swap_names_triggered();
    void on_btnSwapNames_clicked();
    void on_tvLeft_clicked(const QModelIndex&);
    void on_tvRight_clicked(const QModelIndex&);
    void on_action_Sort_toggled(bool needSort);
    void on_btnSort_clicked();
    void on_action_OpenFile_triggered();
    void on_action_OpenDir_triggered();
    void on_action_Save_triggered();
    void on_action_SaveAsFile_triggered();
    void rowDoubleClicked(const QModelIndex&);
    void on_actionSettings_triggered();
    void on_action_Close_triggered();
    void on_leFilterLeft_textChanged(const QString &arg1);
    void on_leFilterRight_textChanged(const QString &arg1);
    void on_action_Filter_triggered();
    void on_actionCompare_Result_triggered();
    void on_actionS_plit_names_triggered();
    void on_action_Drop_slashes_triggered();
    void on_action_Generate_full_name_triggered();
    void on_action_Join_triggered();
    void on_actionDrop_full_name_triggered();
    void on_actionSp_lit_triggered();
    void on_actionIntl_phone_prefix_triggered();
    void on_actionS_wap_Panels_triggered();
    void on_action_SaveAsDir_triggered();

private:
    Ui::MainWindow *ui;
    ContactModel *modLeft, *modRight;
    // Three potentially unsafe pointers (covered by selectView() in all changed)
    QTableView* selectedView;
    ContactModel* selectedModel;
    QLabel* selectedHeader;
    ContactSorterFilter *proxyLeft, *proxyRight;
    // End of potentially unsafe pointers
    QModelIndexList selection;
    SettingsDialog* setDlg;
    QLabel* lbMode;
    void selectView(QTableView* view);
    bool checkSelection(bool errorIfNoSelected = true, bool onlyOneRowAllowed = false);
    void setSorting(bool needSort);
    void setLanguage(const QString& language);
    void updateListHeader(ContactModel* model, QLabel* header);
    void updateHeaders();
    void updateModeStatus();
    void updateViewMode();
    void setSelectionModelEvents();
    inline ContactModel* oppositeModel();
    void askSaveChanges(QCloseEvent *event, ContactModel *model);
    void updateConfig();
};

#endif // MAINWINDOW_H
