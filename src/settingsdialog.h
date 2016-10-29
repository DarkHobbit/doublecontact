#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QStringList>

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
    bool setData();
    bool getData();
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
};

#endif // SETTINGSDIALOG_H
