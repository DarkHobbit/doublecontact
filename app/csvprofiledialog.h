/* Double Contact
 *
 * Module: CSV profile selection dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef CSVPROFILEDIALOG_H
#define CSVPROFILEDIALOG_H

#include <QDialog>
#include "../core/formats/files/csvfile.h"

namespace Ui {
class CSVProfileDialog;
}

class CSVProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CSVProfileDialog(QWidget *parent = 0);
    ~CSVProfileDialog();
    void selectProfile(CSVFile *format);
private slots:
    void on_cbProfile_currentIndexChanged(int);
private:
    Ui::CSVProfileDialog *ui;
};

#endif // CSVPROFILEDIALOG_H
