/* Double Contact
 *
 * Module: SMS Window
 *
 * Copyright 2020 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */
#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QStatusBar>

#include "contactlist.h"
#include "contactsorterfilter.h"
#include "decodedmessagelist.h"
#include "messagemodel.h"

namespace Ui {
class MessageWindow;
}

class MessageWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MessageWindow(ContactList* contacts);
    ~MessageWindow();

private slots:
    void selectionChanged();
    void on_cbPDU_stateChanged(int);
    void on_cbPDUArchive_stateChanged(int);
    void on_cbBinary_stateChanged(int);
    void on_cbVmessage_stateChanged(int);
    void on_cbVmessageArchive_stateChanged(int);
    void on_cbMergeDups_stateChanged(int);
    void on_cbMergeMultiparts_stateChanged(int);
    void toggleSort();
    void on_actionCopy_text_triggered();
    void on_actionProperties_triggered();
    void on_leFilter_textChanged(const QString &newText);
    void on_btnSaveAs_clicked();
    void on_actionSave_MMS_Files_triggered();
    void onShowMMSFile(bool);

protected:
    void showEvent(QShowEvent*);

private:
    Ui::MessageWindow *ui;
    MessageSourceFlags srcFlags;
    MessageModel* model;
    ContactSorterFilter *proxy;
    QStatusBar* statusBar;
    QLabel *lbCount, *lbMMSCount, *lbMode, *lbDups, *lbMultiParts;
    QMenu* menuMMSFiles;
    void setSorting(bool needSort);
    void updateModel();
    void updateStatus();
    void checkButtons();
    void checkMergeButton();
    QModelIndex selectedRecord(bool errorIfNoSelected = true);
};

#endif // MESSAGEWINDOW_H
