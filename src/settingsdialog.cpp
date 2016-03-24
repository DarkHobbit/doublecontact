#include <QApplication>
#include <QMessageBox>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    // Settings settings :)
    // Test data ==>
    ui->lwAvailableColumns->addItem("col1");
    ui->lwAvailableColumns->addItem("col2");
    ui->lwAvailableColumns->addItem("col3");
    ui->lwAvailableColumns->addItem("col4");
    ui->lwAvailableColumns->addItem("col5");
    // TODO load list
    // TODO if list is empty, set default
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::readConfig()
{
    // Language
    QString lang = settings.value("General/Language", "Unknown").toString();
    if (lang==tr("Unknown")) {
        lang = "English"; // TODO use system language
    }
    // Treat name ... as surname
    int surnameIndex = settings.value("General/SurnameIndex", -1).toInt();
    ui->cbSurname->setChecked(surnameIndex>-1);
    ui->sbSurname->setEnabled(surnameIndex>-1);
    if (surnameIndex>-1)
        ui->sbSurname->setValue(surnameIndex);
    // Column view
    int visibleColumnCount = settings.value("VisibleColumns/Count", 0).toInt();
    QStringList validColumnNames; // TODO fill it from array (defs.h)
    for (int i=0; i<visibleColumnCount; i++) { // Fill visible columns list
        QString columnCandidate = settings.value(QString("VisibleColumns/Column%1").arg(i+1)).toString();
        if (validColumnNames.contains(columnCandidate))
            ui->lwVisibleColumns->addItem(columnCandidate);
    }
    for (int i=0; i<validColumnNames.count(); i++) // Fill available columns list
        if (ui->lwVisibleColumns->findItems(validColumnNames[i], Qt::MatchCaseSensitive).isEmpty())
            ui->lwAvailableColumns->addItem(validColumnNames[i]);
}

bool SettingsDialog::writeConfig()
{
    // TODO
}

void SettingsDialog::on_cbSurname_toggled(bool checked)
{
    ui->sbSurname->setEnabled(checked);
}

void SettingsDialog::on_btnAddCol_clicked()
{
    foreach (QListWidgetItem* item, ui->lwAvailableColumns->selectedItems()) {
        ui->lwVisibleColumns->addItem(item->text());
        delete item;
    }
}

void SettingsDialog::on_btnDelCol_clicked()
{
    if (ui->lwVisibleColumns->selectedItems().count()>=ui->lwVisibleColumns->count()) {
        QMessageBox::critical(0, tr("Error"), tr("List must contain at least one visible column"));
    }
    else
    foreach (QListWidgetItem* item, ui->lwVisibleColumns->selectedItems()) {
        ui->lwAvailableColumns->addItem(item->text());
        delete item;
    }
}

void SettingsDialog::on_btnUpCol_clicked()
{
    for (int i=1; i<ui->lwVisibleColumns->count(); i++) {
        QListWidgetItem* item = ui->lwVisibleColumns->item(i);
        if (item->isSelected()) {
            QString colName = item->text();
            delete item;
            ui->lwVisibleColumns->insertItem(i-1, colName);
            ui->lwVisibleColumns->item(i-1)->setSelected(true);
        }
    }
}

void SettingsDialog::on_btnDownCol_clicked()
{
    for (int i=ui->lwVisibleColumns->count()-2; i>=0; i--) {
        QListWidgetItem* item = ui->lwVisibleColumns->item(i);
        if (item->isSelected()) {
            QString colName = item->text();
            delete item;
            ui->lwVisibleColumns->insertItem(i+1, colName);
            ui->lwVisibleColumns->item(i+1)->setSelected(true);
        }
    }
}
