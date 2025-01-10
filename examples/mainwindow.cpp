#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <game.h>
#include <chessbase.h>
#include <dbasepool.h>
#include <QFile>
#include <QLibraryInfo>

static Game * scratchGame = NULL;   

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect ( ui->pushButtonTest,SIGNAL(clicked(bool)),this,SLOT(Test()));
    connect ( ui->pushButtonQuit,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect (ui->actionRemove_the_Test_DataBase,SIGNAL(triggered(bool)),this,SLOT(RemoveTestBase()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RemoveTestBase()
{
    QFile::remove("/home/gilles/Developpements/PGN/test.si5");
    QFile::remove("/home/gilles/Developpements/PGN/test.sg5");
    QFile::remove("/home/gilles/Developpements/PGN/test.sn5");
}

void MainWindow::Test()
{
    DBasePool::init()  ;
    ChessBase cb(this,ui->progressBar);
    connect (&cb,SIGNAL(refreshProgressBar(int,int,Progress::CountType)),
            this,SLOT(refreshProgressBar(int,int,Progress::CountType)));
    int numberbase;
    int code=cb.open(QString("/home/gilles/Developpements/PGN/test"),ICodecDatabase::SCID5,FMODE_Create,numberbase);
    ui->labelError->setText(QString("Open %1 - %2").arg(cb.ErrorCode(code)).arg(numberbase));
    if ( code == OK) {
    scidBaseT *dbase =DBasePool::getBase(numberbase);
    int numberfound;
    code=cb.importGames(dbase,"/home/gilles/Developpements/PGN/00036980Parties.pgn",numberfound);
    if ( code != OK)   ui->labelError->setText(QString("Import %1 - %2").arg(cb.ErrorCode(code)).arg(numberfound));
    else ui->labelError->setText("OK");

    }
}

void MainWindow::refredProgressBar(int value, int total, int type)

{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(total);
    if ( type  == Progress::Sum ){
        ui->progressBar->setFormat("%v");
        ui->progressBar->setValue(value);
    }
    else if ( type  == Progress::SumTotal) {
        ui->progressBar->setFormat("%v/%m");
        ui->progressBar->setValue(value);
    }
    else {
        ui->progressBar->setFormat("%p%");
    ui->progressBar->setValue(value);
    }
}
