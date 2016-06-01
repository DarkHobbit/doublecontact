#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include "comparecontainers.h"

namespace Ui {
class CompareDialog;
}

class CompareDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CompareDialog(QWidget *parent = 0);
    ~CompareDialog();
    void setData(const ContactItem& left, const ContactItem& right);
    void getData(ContactItem& left, ContactItem& right);
private:
    Ui::CompareDialog *ui;
    StringPair* pFullName;
    StringListPair* pNames;
    PhonesPair* pPhones;
    EmailsPair* pEmails;
    // TODO bday, anniversaries
    StringPair* pDesc;
    // TODO photo*
    StringPair* pOrg, *pTitle;
    // TODO address
    template<class P, class T>
    void checkPair(const QString& title, P** pair, T& leftData, T& rightData);
};

#endif // COMPAREDIALOG_H
