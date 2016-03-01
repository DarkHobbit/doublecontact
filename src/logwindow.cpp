#include "logwindow.h"
#include "ui_logwindow.h"

LogWindow::LogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::setData(const QString& path, const ContactList &items, const QStringList &errors)
{
    setWindowTitle(tr("File %1").arg(path));
    ui->lbSummary->setText(tr("%1 records has been read").arg(items.count()));
    ui->lwErrors->clear();
    ui->lwErrors->addItems(errors);
}

void LogWindow::changeEvent(QEvent *e)
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
