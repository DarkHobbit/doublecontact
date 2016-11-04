/* Double Contact
 *
 * Module: Multi contact editing dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef MULTICONTACTDIALOG_H
#define MULTICONTACTDIALOG_H

#include <QDialog>
#include "contactlist.h"

namespace Ui {
class MultiContactDialog;
}

class MultiContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiContactDialog(QWidget *parent = 0);
    ~MultiContactDialog();
    void setData(const ContactItem& item, bool firstItem);
    void getData(ContactItem& item);
private:
    Ui::MultiContactDialog *ui;
    // TODO highlight * as gray?
};

#endif // MULTICONTACTDIALOG_H
