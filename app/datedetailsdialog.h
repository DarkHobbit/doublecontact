/* Double Contact
 *
 * Module: Date details dialog (time presence, time zone)
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef DATEDETAILSDIALOG_H
#define DATEDETAILSDIALOG_H

#include <QDateTimeEdit>
#include <QDialog>
#include "contactlist.h"

namespace Ui {
class DateDetailsDialog;
}

class DateDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DateDetailsDialog(QWidget *parent = 0);
    ~DateDetailsDialog();
    void setData(const DateItem& data);
    void getData(DateItem& data);
    static void setDateFormat(QDateTimeEdit* editor, bool hasTime);
private slots:
    void on_cbHasTime_toggled(bool checked);
    void on_cbHasTimeZone_toggled(bool checked);
private:
    Ui::DateDetailsDialog *ui;
};

#endif // DATEDETAILSDIALOG_H
