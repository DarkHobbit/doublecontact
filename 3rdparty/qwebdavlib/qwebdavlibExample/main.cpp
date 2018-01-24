#include <QtCore/QCoreApplication>

#include "qexample.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QExample e;
    e.start();
    
    return a.exec();
}
