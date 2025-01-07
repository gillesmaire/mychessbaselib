#include <QApplication>

#include "game.h"
#include "mainwindow.h"
#include <QDebug>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow; 
    mainWindow.setWindowTitle("Application Qt Minimal");
    mainWindow.resize(800, 600); 
    mainWindow.show();
  
    return a.exec();
}
