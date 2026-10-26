#include "mainwindow.h"
#include <QApplication>

MainWindow* w_p = NULL;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w_p = &w;


    w.show();
    return a.exec();
}
