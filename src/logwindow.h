#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>
#include <QStringList>
#include "contactlist.h"

namespace Ui {
class LogWindow;
}

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = 0);
    ~LogWindow();
    void setData(const QString& path, const ContactList& items, const QStringList& errors);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
