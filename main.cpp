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
    int code=cb.open("/home/gilles/Test/test",ICodecDatabase::SCID5,FMODE_Create,number);
    qDebug()<<code<<number;
    scidBaseT *dbase =DBasePool::getBase(number);
    code=cb.importGames(dbase,"/home/gilles/Developpements/PGN/00003265parties.pgn",number);
    qDebug()<<code<<number;
    

    return a.exec();
}
