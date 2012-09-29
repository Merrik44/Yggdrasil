#include <QtGui/QApplication>
#include "mainwindow.h"
#include "iostream"

int main(int argc, char *argv[])
{
    if( argc > 0 )
        std::cout << argc << std::endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
