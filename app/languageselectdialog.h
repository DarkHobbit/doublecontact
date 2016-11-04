/* Double Contact
 *
 * Module: Human language select dialog
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#ifndef LANGUAGESELECTDIALOG_H
#define LANGUAGESELECTDIALOG_H

#include <QDialog>

namespace Ui {
class LanguageSelectDialog;
}

class LanguageSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageSelectDialog(QWidget *parent = 0);
    ~LanguageSelectDialog();
    static QString selectLanguage();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::LanguageSelectDialog *ui;
};

#endif // LANGUAGESELECTDIALOG_H
