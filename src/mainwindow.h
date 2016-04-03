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
    virtual void showEvent(QShowEvent* event);
private slots:
    void on_action_Other_panel_triggered();
    void setButtonsAccess();
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
    void on_action_Sort_triggered();
    void on_btnSort_clicked();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_actionSave_as_triggered();
    void rowDoubleClicked(const QModelIndex&);
    void on_actionSettings_triggered();

    void on_action_Close_triggered();

private:
    Ui::MainWindow *ui;
    ContactModel *modLeft, *modRight;
    // Three potentially unsafe pointers (covered by selectView() in all changed)
    QTableView* selectedView;
    ContactModel* selectedModel;
    QLabel* selectedHeader;
    // End of potentially unsafe pointers
    QModelIndexList selection;
    SettingsDialog* setDlg;
    void selectView(QTableView* view);
    bool checkSelection();
    void updateListHeader(ContactModel* model, QLabel* header);
    void updateHeaders();
    inline ContactModel* oppositeModel();
    void askSaveChanges(QCloseEvent *event, QTableView* view);
    void updateConfig();
};

#endif // MAINWINDOW_H
