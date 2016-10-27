#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVector>

#include "../gui/configmanager.h"
#include "globals.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    // Common configuration, managed by dialog
    bool readConfig();
    bool writeConfig();
    ContactColumnList columnNames();
    QString lang();
    bool langChanged();
private slots:
    void on_btnAddCol_clicked();
    void on_btnDelCol_clicked();
    void on_btnUpCol_clicked();
    void on_btnDownCol_clicked();
    void on_cbUseSystemDateTimeFormat_clicked(bool checked);

private:
    QString _lang;
    bool _langChanged;
    Ui::SettingsDialog *ui;
    QSettings settings;
    QStringList validColumnNames; // Available list
    void updateGlobalData();
};

#endif // SETTINGSDIALOG_H
