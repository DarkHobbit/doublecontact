/* Double Contact
 *
 * Module: Export/Import log window
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>
#include <QStringList>
#include "contactlist.h"

namespace Ui {
class LogWindow;
}

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = 0);
    ~LogWindow();
    void setData(const QString& path, const ContactList& items, const QStringList& errors);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_btnToClipboard_clicked();

private:
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
