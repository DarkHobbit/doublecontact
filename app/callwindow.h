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
#ifndef CALLWINDOW_H
#define CALLWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QStatusBar>

#include "contactlist.h"
#include "callmodel.h"

namespace Ui {
class CallWindow;
}

class CallWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CallWindow(ContactList* contacts);
    ~CallWindow();

private slots:
    void toggleSort();
    void onSortIndicatorChanged(int,Qt::SortOrder);
    void on_actionCopy_text_triggered();
    void on_leFilter_textChanged(const QString &newText);
    void on_btnSaveAs_clicked();

protected:
    void showEvent(QShowEvent*);

private:
    Ui::CallWindow *ui;
    CallModel* model;
    QSortFilterProxyModel *proxy;
    QStatusBar* statusBar;
    QLabel *lbCount, *lbMode;
    void updateModel();
    void updateStatus();
    QModelIndex selectedRecord(bool errorIfNoSelected = true);
};

#endif // CALLWINDOW_H
