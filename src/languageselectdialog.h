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
