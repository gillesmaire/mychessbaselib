#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <game.h>
#include <chessbase.h>
#include <dbasepool.h>
static Game * scratchGame = NULL;   

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect ( ui->pushButtonTest,SIGNAL(clicked(bool)),this,SLOT(Test()));
    
   
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Test()
{
    DBasePool::init()  ;
    ChessBase cb(this,ui->progressBar);
    int number;
    int code=cb.open("/home/gilles/Test/test",ICodecDatabase::SCID5,FMODE_Create,number);
    if ( code != OK)   ui->labelError->setText(QString("%1 - %2").arg(cb.ErrorCode(code)).arg(number));
    else {
    scidBaseT *dbase =DBasePool::getBase(number);
    code=cb.importGames(dbase,"/home/gilles/Developpements/PGN/00003265parties.pgn",number);
    if ( code != OK)   ui->labelError->setText(QString("%1 - %2").arg(cb.ErrorCode(code)).arg(number));
    else ui->labelError->setText("Ok");
    }
}
