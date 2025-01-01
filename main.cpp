#include <QApplication>
#include "game.h"
#include "chessbase.h"
#include <QMainWindow>
#include <dbasepool.h>

static Game * scratchGame = NULL;   

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow mainWindow; 
    mainWindow.setWindowTitle("Application Qt Minimal");
    mainWindow.resize(800, 600); 
    mainWindow.show();
    DBasePool::init()  ;
    ChessBase cb(&mainWindow);
    int numberbase;
    cb.create("/tmp/mabase",ICodecDatabase::SCID5,numberbase);
    
    

    return a.exec();
}
