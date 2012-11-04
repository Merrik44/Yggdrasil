#include <QtGui/QApplication>
#include "leafwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LeafWindow w;
    w.show();
    
    return a.exec();
}
