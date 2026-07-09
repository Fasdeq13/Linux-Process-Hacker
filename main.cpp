#include <QApplication>
#include <QMetaType>
#include <QSet>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSet<int>>("QSet<int>");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    QApplication application(argc, argv);
    application.setApplicationName("LinuxProcessHacker");
    application.setOrganizationName("LinuxProcessHacker");

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
