#include <QApplication>
#include "mainwindow.h"
#include "dbasepool.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBasePool::init()  ;
    MainWindow mainWindow; 
    mainWindow.setWindowTitle("Application Qt Minimal");
    mainWindow.resize(800, 600); 
    mainWindow.show();
    MainWindow::createDir();
   
    return a.exec();
}
