#include <QPalette>
#include <QVBoxLayout>
#include "comparecontainers.h"

ItemPair::ItemPair(const QString& title, QGridLayout* layout)
    :QObject(layout)
{
    int oldRowCount = layout->rowCount();
    gbLeft = new QGroupBox(title);
    gbRight = new QGroupBox(title);
    btnToLeft = new QToolButton();
    btnToLeft->setArrowType(Qt::LeftArrow);
    btnToRight = new QToolButton();
    btnToRight->setArrowType(Qt::RightArrow);
    layout->addWidget(gbLeft, oldRowCount, 0);
    layout->addWidget(btnToLeft, oldRowCount, 1);
    layout->addWidget(btnToRight, oldRowCount, 2);
    layout->addWidget(gbRight, oldRowCount, 3);
    connect(btnToLeft, SIGNAL(clicked()), this, SLOT(onToLeftClicked()));
    connect(btnToRight, SIGNAL(clicked()), this, SLOT(onToRightClicked()));
}

void ItemPair::highlightDiff(bool hasDiff)
{
    QString sheet =
        QString("QGroupBox { border: 1px groove %1; border-radius: 2px; }")
        .arg(hasDiff ? "red" : "green");
    gbLeft->setStyleSheet(sheet);
    gbRight->setStyleSheet(sheet);
}


void ItemPair::onToLeftClicked()
{
    copyData(true);
    highlightDiff(false);
}

void ItemPair::onToRightClicked()
{
    copyData(false);
    highlightDiff(false);
}

StringListPair::StringListPair(const QString& title, QGridLayout *layout, const QStringList &leftData, const QStringList &rightData)
    :ItemPair(title, layout)
{
    fillBox(gbLeft, leftData, leftSet);
    fillBox(gbRight, rightData, rightSet);
    highlightDiff(leftData!=rightData);
}

void StringListPair::getData(QStringList &leftData, QStringList &rightData)
{
    leftData.clear();
    foreach(QLineEdit* ed, leftSet)
        leftData.push_back(ed->text());
    rightData.clear();
    foreach(QLineEdit* ed, rightSet)
        rightData.push_back(ed->text());
}

void StringListPair::copyData(bool toLeft)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftSet : rightSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightSet : leftSet;
    QLayout *layDest = (toLeft ? gbLeft : gbRight)->layout();
    while (edSetDest.count()>edSetSrc.count())
    {
        delete edSetDest.last();
        edSetDest.removeLast();
    }
    while (edSetDest.count()<edSetSrc.count()) {
        edSetDest.push_back(new QLineEdit());
        layDest->addWidget(edSetDest.last());
    }
    for (int i=0; i<edSetDest.count(); i++)
        edSetDest[i]->setText(edSetSrc[i]->text());
}

void StringListPair::fillBox(QGroupBox *gBox, const QStringList &data, QList<QLineEdit*>& edSet)
{
    QVBoxLayout* layEditors = new QVBoxLayout(gBox);
    foreach(const QString& s, data) {
        edSet.push_back(new QLineEdit(s));
        layEditors->addWidget(edSet.last());
    }
}


StringPair::StringPair(const QString &title, QGridLayout *layout, const QString &leftData, const QString &rightData)
    :StringListPair(title, layout, QStringList(leftData), QStringList(rightData))
{
}

void StringPair::getData(QString &leftData, QString &rightData)
{
    QStringList ll, rl;
    StringListPair::getData(ll, rl);
    leftData = ll[0];
    rightData = rl[0];
}

TypedPair::TypedPair(const QString &title, QGridLayout *layout)
    :ItemPair(title, layout)
{
    layLeft = new QGridLayout(gbLeft);
    layRight = new QGridLayout(gbRight);
}

void TypedPair::addValue(const QString &value, const QStringList &types, bool toLeft)
{
    QGridLayout* layout = toLeft ? layLeft : layRight;
    QList<QLineEdit*>& edSet = toLeft ? leftEdSet : rightEdSet;
    QList<QComboBox*>& comboSet = toLeft ? leftComboSet : rightComboSet;
    int prevCount = edSet.count();
    edSet.push_back(new QLineEdit(value));
    layout->addWidget(edSet.last(), prevCount, 0);
    comboSet.push_back(new QComboBox());
    comboSet.last()->addItems(types);
    layout->addWidget(comboSet.last(), prevCount, 1);
}

int TypedPair::count(bool onLeft)
{
    return (onLeft ? layLeft : layRight)->rowCount();
}

bool TypedPair::getValue(int index, QString &value, QStringList &types, bool fromLeft)
{
    QList<QLineEdit*>& edSet = fromLeft ? leftEdSet : rightEdSet;
    QList<QComboBox*>& comboSet = fromLeft ? leftComboSet : rightComboSet;
    if (index>=edSet.count()) return false;
    value = edSet[index]->text();
    types.clear();
    for (int i=0; i<comboSet[index]->count(); i++)
        types << comboSet[index]->itemText(i);
    return true;
}

void TypedPair::copyData(bool toLeft)
{
    QList<QLineEdit*>& edSetDest = toLeft ? leftEdSet : rightEdSet;
    QList<QLineEdit*>& edSetSrc = toLeft ? rightEdSet : leftEdSet;
    QList<QComboBox*>& comboSetDest = toLeft ? leftComboSet : rightComboSet;
    QList<QComboBox*>& comboSetSrc = toLeft ? rightComboSet : leftComboSet;
    QGridLayout* layDest = dynamic_cast<QGridLayout*>((toLeft ? gbLeft : gbRight)->layout());
    while (edSetDest.count()>edSetSrc.count())
    {
        delete edSetDest.last();
        edSetDest.removeLast();
        delete comboSetDest.last();
        comboSetDest.removeLast();
    }
    while (edSetDest.count()<edSetSrc.count()) {
        int prevCount = layDest->rowCount();
        edSetDest.push_back(new QLineEdit());
        layDest->addWidget(edSetDest.last(), prevCount, 0);
        comboSetDest.push_back(new QComboBox());
        layDest->addWidget(comboSetDest.last(), prevCount, 1);
    }
    for (int i=0; i<edSetDest.count(); i++) {
        edSetDest[i]->setText(edSetSrc[i]->text());
        comboSetDest[i]->clear();
        for (int j=0; j<comboSetSrc[i]->count(); j++)
            comboSetDest[i]->addItem(comboSetSrc[i]->itemText(j));
    }
}

PhonesPair::PhonesPair(const QString &title, QGridLayout *layout, const QList<Phone> &leftPhones, const QList<Phone> &rightPhones)
    :TypedPair(title, layout)
{
    foreach(const Phone& p, leftPhones)
        addValue(p.number, p.tTypes, true);
    foreach(const Phone& p, rightPhones)
        addValue(p.number, p.tTypes, false);
    highlightDiff(leftPhones!=rightPhones);
}

void PhonesPair::getData(QList<Phone> &leftPhones, QList<Phone> &rightPhones)
{
    leftPhones.clear();
    for (int i=0; i<count(true); i++) {
        Phone p;
        getValue(i, p.number, p.tTypes, true);
        leftPhones << p;
    }
    rightPhones.clear();
    for (int i=0; i<count(false); i++) {
        Phone p;
        getValue(i, p.number, p.tTypes, false);
        rightPhones << p;
    }
}

EmailsPair::EmailsPair(const QString &title, QGridLayout *layout, const QList<Email> &leftEmails, const QList<Email> &rightEmails)
    :TypedPair(title, layout)
{
    foreach(const Email& p, leftEmails)
        addValue(p.address, p.emTypes, true);
    foreach(const Email& p, rightEmails)
        addValue(p.address, p.emTypes, false);
    highlightDiff(leftEmails!=rightEmails);
}

void EmailsPair::getData(QList<Email> &leftEmails, QList<Email> &rightEmails)
{
    leftEmails.clear();
    for (int i=0; i<count(true); i++) {
        Email p;
        getValue(i, p.address, p.emTypes, true);
        leftEmails << p;
    }
    rightEmails.clear();
    for (int i=0; i<count(false); i++) {
        Email p;
        getValue(i, p.address, p.emTypes, false);
        rightEmails << p;
    }
}
