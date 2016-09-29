#include "languagemanager.h"
#include "languageselectdialog.h"
#include "ui_languageselectdialog.h"

LanguageSelectDialog::LanguageSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LanguageSelectDialog)
{
    ui->setupUi(this);
}

LanguageSelectDialog::~LanguageSelectDialog()
{
    delete ui;
}

QString LanguageSelectDialog::selectLanguage()
{
    QString language = languageManager.systemLanguageNativeName();
    LanguageSelectDialog* d = new LanguageSelectDialog(0);
    d->ui->cbLanguage->addItems(languageManager.nativeNames());
    int index = d->ui->cbLanguage->findText(language);
    if (index!=-1)
        d->ui->cbLanguage->setCurrentIndex(index);
    d->exec();
    language = d->ui->cbLanguage->currentText();
    delete d;
    return language;
}

void LanguageSelectDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
