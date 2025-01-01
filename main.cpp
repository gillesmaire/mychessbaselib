#include <QApplication>
#include "game.h"
#include "chessbase.h"
#include <QMainWindow>
#include <dbasepool.h>
#include <QDebug>


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
    int number;
    int code=cb.open("/home/gilles/test3.pgn",ICodecDatabase::PGN,FMODE_Create,number);
    
    int bp=DBasePool::find("/home/gilles/test3.pgn");
    qDebug()<<bp;
    
    

    return a.exec();
}
