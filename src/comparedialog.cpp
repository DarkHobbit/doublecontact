#include <QFrame>

#include "comparedialog.h"
#include "ui_comparedialog.h"

#include <QDebug>

CompareDialog::CompareDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompareDialog),
    pFullName(0), pNames(0)
{
    ui->setupUi(this);
}

CompareDialog::~CompareDialog()
{
    delete ui;
}

template<class P, class T>
void CompareDialog::checkPair(const QString& title, P** pair, T& leftData, T& rightData)
{
    *pair = 0;
    if (!leftData.isEmpty() || !rightData.isEmpty())
        *pair = new P(title, ui->layPairs, leftData, rightData);
}

void CompareDialog::setData(const ContactItem &left, const ContactItem &right)
{
    ui->layPairs->setAlignment(Qt::AlignTop);

    checkPair(tr("Full Name"), &pFullName, left.fullName, right.fullName);
    /*if (!left.fullName.isEmpty() || !right.fullName.isEmpty())
        pFullName = new StringPair(tr("Full Name"), ui->layPairs, left.fullName, right.fullName);
    else pFullName = 0;*/

    if (!left.names.isEmpty() || !right.names.isEmpty())
        pNames = new StringListPair(tr("Names"), ui->layPairs, left.names, left.names);
    else pNames = 0;

    if (!left.phones.isEmpty() || !right.phones.isEmpty())
        pPhones = new PhonesPair(tr("Phones"), ui->layPairs, left.phones, right.phones);
    else pPhones = 0;

    // TODO
}


void CompareDialog::getData(ContactItem &left, ContactItem &right)
{
    if (pFullName)
        pFullName->getData(left.fullName, right.fullName);
    if (pNames)
        pNames->getData(left.names, right.names);
    if (pPhones)
        pPhones->getData(left.phones, right.phones);
    // TODO
}
