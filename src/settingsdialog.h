#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QVector>
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
    bool readConfig();
    bool writeConfig();
    QString lastPath();
    void setLastPath(const QString& path);
    bool showTwoPanels();
    void setShowTwoPanels(bool value);
    bool openLastFilesAtStartup();
    void setOpenLastFilesAtStartup();
    ContactColumnList columnNames();
private slots:
    void on_cbSurname_toggled(bool checked);
    void on_btnAddCol_clicked();
    void on_btnDelCol_clicked();
    void on_btnUpCol_clicked();
    void on_btnDownCol_clicked();
private:
    Ui::SettingsDialog *ui;
    QSettings settings;
    QStringList validColumnNames; // Available list
};

#endif // SETTINGSDIALOG_H
