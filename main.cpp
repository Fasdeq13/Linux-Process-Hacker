#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("LinuxProcessHacker");
    application.setOrganizationName("LinuxProcessHacker");

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
