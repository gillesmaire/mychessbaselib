#include <QApplication>
#include "mainwindow.h"
#include "dbasepool.h"


// This program  use the port Scid5lib for Qt6 
// It tests and demonstrates all the library funtions.
// It shows also some additional feature : 
//     - how to write a scid5baseModel to display database view
//     - how to play with DBasePool class
//     - how to use filters on games
// To check some feature you will need to use scid program 
// to Open Library. 
// Use a developement scid using scid5 format and not an old 
// program using only scid4. Actually in 2025 debian 12 provides
// only a scid4 release.
//


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
