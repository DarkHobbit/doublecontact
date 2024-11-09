#include <QApplication>
#include <QColorDialog>
#include <QFont>
#include <QFontDialog>
#include <QMessageBox>

#include "configmanager.h"
#include "contactlist.h"
#include "languagemanager.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    _lang(""), _langChanged(false), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->bgNlTnPolicy->setId(ui->rbSaveNLNSNamesAsIs, 0);
    ui->bgNlTnPolicy->setId(ui->rbReplaceNLNSNames, 1);
    ui->bgNlTnPolicy->setId(ui->rbXCustomizeNLNSNames, 2);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::setData()
{
    // Language
    ui->cbLanguage->insertItems(0, languageManager.nativeNames());
    _lang = configManager.readLanguage();
    ui->cbLanguage->setCurrentIndex(ui->cbLanguage->findText(_lang));
    // Locale
    ui->leDateFormat->setText(gd.dateFormat);
    ui->leTimeFormat->setText(gd.timeFormat);
    ui->cbUseSystemDateTimeFormat->setChecked(gd.useSystemDateTimeFormat);
    on_cbUseSystemDateTimeFormat_clicked(ui->cbUseSystemDateTimeFormat->isChecked());
    // Misc
    ui->cbOpenLastFilesAtStartup->setChecked(gd.openLastFilesAtStartup);
    // View
    ui->cbShowTableGrid->setChecked(gd.showTableGrid);
    ui->cbShowLineNumbers->setChecked(gd.showLineNumbers);
    ui->cbResizeTableRowsToContents->setChecked(gd.resizeTableRowsToContents);
    ui->cbUseTableAlternateColors->setChecked(gd.useTableAlternateColors);
    ui->cbUseSystemFontsAndColors->setChecked(gd.useSystemFontsAndColors);
    on_cbUseSystemFontsAndColors_clicked(ui->cbUseSystemFontsAndColors->isChecked());
    // Column view
    for (int i=0; i<gd.columnNames.count(); i++) // Fill visible columns list
            ui->lwVisibleColumns->addItem(contactColumnHeaders[gd.columnNames[i]]);
    for (int i=0; i<configManager.validColumnNames.count(); i++) // Fill available columns list
        if (ui->lwVisibleColumns->findItems(configManager.validColumnNames[i], Qt::MatchCaseSensitive).isEmpty())
            ui->lwAvailableColumns->addItem(configManager.validColumnNames[i]);
    // Saving
    ui->cbPrefVCardVer->addItems(enPrefVCFVersion.possibleValues());
    ui->cbPrefVCardVer->setCurrentIndex((short)gd.preferredVCFVersion);
    ui->cbUseOrigVer->setChecked(gd.useOriginalFileVersion);
    ui->cbWriteFullNameIsEmpty->setChecked(gd.writeFullNameIsEmpty);
    ui->cbDefaultCountryRules->clear();
    ui->cbDefaultCountryRules->addItems(Phone::availableCountryRules());
    if (gd.defaultCountryRule>=0 && gd.defaultCountryRule<COUNTRY_RULES_COUNT)
        ui->cbDefaultCountryRules->setCurrentIndex(gd.defaultCountryRule);
    ui->cbSkipTimeFromDate->setChecked(gd.skipTimeFromDate);
    ui->cbAddXToNonStandardTypes->setChecked(gd.addXToNonStandardTypes);
    ui->bgNlTnPolicy->button((int)gd.nonLatinTypeNamesPolicy)->setChecked(true); // local-unsafe but setData() called only with checked values
    ui->cbGroupFormat->addItems(enGroupFormat.possibleValues());
    ui->cbGroupFormat->setCurrentIndex((short)gd.groupFormat);
    // Loading
    ui->cbDefaultEmptyPhoneType->clear();
    ui->cbDefaultEmptyPhoneType->insertItems(0, Phone::standardTypes.displayValues);
    int index = ui->cbDefaultEmptyPhoneType->findText(gd.defaultEmptyPhoneType);
    if (index!=-1)
        ui->cbDefaultEmptyPhoneType->setCurrentIndex(index);
    ui->cbWarnOnMissingTypes->setChecked(gd.warnOnMissingTypes);
    ui->cbWarnOnNonStandardTypes->setChecked(gd.warnOnNonStandardTypes);
    ui->cbReadNamesFromFileName->setChecked(gd.readNamesFromFileName);
    ui->cbDebugSave->setChecked(gd.debugSave);
    // Done
    return true;
}

bool SettingsDialog::getData()
{
    // Language
    QString newLang = ui->cbLanguage->currentText();
    configManager.writeLanguage(newLang);
    if (newLang!=_lang) {
        _langChanged = true;
        _lang = newLang;
    }
    // Locale
    gd.dateFormat = ui->leDateFormat->text();
    gd.timeFormat = ui->leTimeFormat->text();
    gd.useSystemDateTimeFormat = ui->cbUseSystemDateTimeFormat->isChecked();
    // Misc
    gd.openLastFilesAtStartup = ui->cbOpenLastFilesAtStartup->isChecked();
    // View
    gd.showTableGrid = ui->cbShowTableGrid->isChecked();
    gd.showLineNumbers = ui->cbShowLineNumbers->isChecked();
    gd.resizeTableRowsToContents = ui->cbResizeTableRowsToContents->isChecked();
    gd.useTableAlternateColors = ui->cbUseTableAlternateColors->isChecked();
    gd.useSystemFontsAndColors = ui->cbUseSystemFontsAndColors->isChecked();
    // Column view
    gd.columnNames.clear();
    for (int i=0; i<ui->lwVisibleColumns->count(); i++)
        gd.columnNames << (ContactColumn)configManager.validColumnNames.indexOf(ui->lwVisibleColumns->item(i)->text());
    // Saving
    gd.preferredVCFVersion = (GlobalConfig::VCFVersion) ui->cbPrefVCardVer->currentIndex();
    gd.useOriginalFileVersion = ui->cbUseOrigVer->isChecked();
    gd.useOriginalFileVersion = ui->cbWriteFullNameIsEmpty->isChecked();
    gd.defaultCountryRule = ui->cbDefaultCountryRules->currentIndex();
    gd.skipTimeFromDate = ui->cbSkipTimeFromDate->isChecked();
    gd.addXToNonStandardTypes = ui->cbAddXToNonStandardTypes->isChecked();
    gd.nonLatinTypeNamesPolicy =
        (GlobalConfig::NonLatinTypeNamesPolicy) ui->bgNlTnPolicy->checkedId();
    gd.groupFormat = (GlobalConfig::GroupFormat)ui->cbGroupFormat->currentIndex();
    // Loading
    gd.defaultEmptyPhoneType = ui->cbDefaultEmptyPhoneType->currentText();
    gd.warnOnMissingTypes = ui->cbWarnOnMissingTypes->isChecked();
    gd.warnOnNonStandardTypes = ui->cbWarnOnNonStandardTypes->isChecked();
    gd.readNamesFromFileName = ui->cbReadNamesFromFileName->isChecked();
    gd.debugSave = ui->cbDebugSave->isChecked();
    // Done
    return true;
}

bool SettingsDialog::langChanged()
{
    return _langChanged;
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
        QMessageBox::critical(0, S_ERROR, tr("List must contain at least one visible column"));
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

void SettingsDialog::on_cbUseSystemDateTimeFormat_clicked(bool checked)
{
    ui->leDateFormat->setEnabled(!checked);
    ui->leTimeFormat->setEnabled(!checked);
}

void SettingsDialog::on_cbUseSystemFontsAndColors_clicked(bool checked)
{
    ui->btnTableFont->setEnabled(!checked);
    ui->btnGridColor1->setEnabled(!checked);
    ui->btnGridColor2->setEnabled(!checked);
}

void SettingsDialog::on_btnTableFont_clicked()
{
    QFontDialog* d = new QFontDialog;
    QFont f;
    f.fromString(gd.tableFont);
    d->setCurrentFont(f);
    if (d->exec()==QDialog::Accepted)
        gd.tableFont = d->currentFont().toString();
    delete d;
}

void SettingsDialog::on_btnGridColor1_clicked()
{
    QColorDialog* d = new QColorDialog;
    d->setCurrentColor(QColor(gd.gridColor1));
    if (d->exec()==QDialog::Accepted)
        gd.gridColor1 = d->currentColor().name();
    delete d;
}

void SettingsDialog::on_btnGridColor2_clicked()
{
    QColorDialog* d = new QColorDialog;
    d->setCurrentColor(QColor(gd.gridColor2));
    if (d->exec()==QDialog::Accepted)
        gd.gridColor2 = d->currentColor().name();
    delete d;
}

void SettingsDialog::on_lwAvailableColumns_itemDoubleClicked(QListWidgetItem *item)
{
    item->setSelected(true);
    on_btnAddCol_clicked();
}


void SettingsDialog::on_lwVisibleColumns_itemDoubleClicked(QListWidgetItem *item)
{
    item->setSelected(true);
    on_btnDelCol_clicked();
}

