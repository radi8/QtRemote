#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("QtProject/QtRemote");
    QCoreApplication::setApplicationName("QtRemote");

    MainWindow w;

    w.show();

    return a.exec();
}
