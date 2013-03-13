#include "flipp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Flipp w;
    w.showMaximized();
    
    return a.exec();
}
