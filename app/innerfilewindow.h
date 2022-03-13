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
#ifndef INNERFILEWINDOW_H
#define INNERFILEWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QStatusBar>

#include "contactlist.h"
#include "innerfilemodel.h"

namespace Ui {
class InnerFileWindow;
}

class InnerFileWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InnerFileWindow(ContactList* contacts);
    ~InnerFileWindow();

private slots:
    void toggleSort();
    void onSortIndicatorChanged(int,Qt::SortOrder);
    void on_actionCopy_text_triggered();
    void on_leFilter_textChanged(const QString &newText);
    void on_btnSaveAll_clicked();

    void on_actionShowFile_triggered();

    void on_actionSave_file_triggered();

protected:
    void showEvent(QShowEvent*);

private:
    Ui::InnerFileWindow *ui;
    InnerFileModel* model;
    QSortFilterProxyModel *proxy;
    QStatusBar* statusBar;
    QLabel *lbCount, *lbMode;
    void updateModel();
    void updateStatus();
    QModelIndex selectedRecord(bool errorIfNoSelected = true);
};

#endif // INNERFILEWINDOW_H
