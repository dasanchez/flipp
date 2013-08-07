#include "flipp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Flipp w;
    w.show();
    
    QCoreApplication::setOrganizationName("daSanchez");
    QCoreApplication::setApplicationName("flipp");

    return a.exec();
}
