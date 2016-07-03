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
    // Common configuration, managed by dialog
    bool readConfig();
    bool writeConfig();
    ContactColumnList columnNames();
    QString lang();
    bool langChanged();
    // Separate settings, managed by main window commands
    QString lastPath();
    void setLastPath(const QString& path);
    bool showTwoPanels();
    void setShowTwoPanels(bool value);
    bool openLastFilesAtStartup();
    void setOpenLastFilesAtStartup();
    bool sortingEnabled();
    void setSortingEnabled(bool value);
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
