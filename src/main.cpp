#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Setup application info for QSettings/Database
    QApplication::setOrganizationName("DeejMix");
    QApplication::setApplicationName("Deej Mix");

    // Stylesheet is loaded and themed by MainWindow via applyAccentColor()

    MainWindow w;
    w.show();
    return a.exec();
}
