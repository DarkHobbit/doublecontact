#include <QApplication>
#include <QMessageBox>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    for (int i=0; i<ccLast; i++)
         validColumnNames << contactColumnHeaders[i];
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
    ui->cbLanguage->setCurrentIndex(ui->cbLanguage->findText(lang));
    // Treat name ... as surname
    int surnameIndex = settings.value("General/SurnameIndex", -1).toInt();
    ui->cbSurname->setChecked(surnameIndex>-1);
    ui->sbSurname->setEnabled(surnameIndex>-1);
    if (surnameIndex>-1)
        ui->sbSurname->setValue(surnameIndex);
    // Misc
    ui->cbOpenLastFilesAtStartup->setChecked(settings.value("General/OpenLastFilesAtStartup", true).toBool());
    // Column view
    int visibleColumnCount = settings.value("VisibleColumns/Count", 0).toInt();
    for (int i=0; i<visibleColumnCount; i++) { // Fill visible columns list
        QString columnCandidate = settings.value(QString("VisibleColumns/Column%1").arg(i+1)).toString();
        if (validColumnNames.contains(columnCandidate))
            ui->lwVisibleColumns->addItem(columnCandidate);
    }
    if (ui->lwVisibleColumns->count()==0) { // if list is empty, set default
        ui->lwVisibleColumns->addItem(contactColumnHeaders[ccFirstName]);
        ui->lwVisibleColumns->addItem(contactColumnHeaders[ccSecondName]);
        ui->lwVisibleColumns->addItem(contactColumnHeaders[ccPhone]);
    }
    for (int i=0; i<validColumnNames.count(); i++) // Fill available columns list
        if (ui->lwVisibleColumns->findItems(validColumnNames[i], Qt::MatchCaseSensitive).isEmpty())
            ui->lwAvailableColumns->addItem(validColumnNames[i]);
    return true;
}

bool SettingsDialog::writeConfig()
{
    // Language
    settings.setValue("General/Language", ui->cbLanguage->currentText());
    // Treat name ... as surname
    settings.setValue("General/SurnameIndex",
        ui->cbSurname->isChecked() ? ui->sbSurname->value()+1 : -1);
    // Misc
    settings.setValue("General/OpenLastFilesAtStartup", ui->cbOpenLastFilesAtStartup->isChecked());
    // Column view
    settings.setValue("VisibleColumns/Count", ui->lwVisibleColumns->count());
    for (int i=0; i<ui->lwVisibleColumns->count(); i++)
        settings.setValue(QString("VisibleColumns/Column%1").arg(i+1),
                          ui->lwVisibleColumns->item(i)->text());
    return true;
}

QString SettingsDialog::lastPath()
{
    QString defaultDir =
#if QT_VERSION >= 0x050000
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
        QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation);
#endif
    return settings.value("General/LastFile", defaultDir).toString();
}

void SettingsDialog::setLastPath(const QString &path)
{
    settings.setValue("General/LastFile", path);
}

bool SettingsDialog::showTwoPanels()
{
    return settings.value("General/ShowTwoPanels", true).toBool();
}

void SettingsDialog::setShowTwoPanels(bool value)
{
    settings.setValue("General/ShowTwoPanels", value);
}

bool SettingsDialog::openLastFilesAtStartup()
{
    return ui->cbOpenLastFilesAtStartup->isChecked();
}

bool SettingsDialog::sortingEnabled()
{
    return settings.value("General/SortingEnabled", false).toBool();
}

void SettingsDialog::setSortingEnabled(bool value)
{
    settings.setValue("General/SortingEnabled", value);

}

ContactColumnList SettingsDialog::columnNames()
{
    ContactColumnList res;
    for (int i=0; i<ui->lwVisibleColumns->count(); i++)
        res << (ContactColumn)validColumnNames.indexOf(ui->lwVisibleColumns->item(i)->text());
    return res;
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

