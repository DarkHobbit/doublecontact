/* Double Contact
 *
 * Module: Phone type selection dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef PHONETYPEDIALOG_H
#define PHONETYPEDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QString>
#include "globals.h"

namespace Ui {
class PhoneTypeDialog;
}

class PhoneTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PhoneTypeDialog(const QString& title, const StandardTypes& sTypes);
    ~PhoneTypeDialog();
    QString getData() const;
    static void selectType(const QString& title, const StandardTypes& sTypes, QComboBox* cbT);
private:
    Ui::PhoneTypeDialog *ui;
};

#endif // PHONETYPEDIALOG_H
