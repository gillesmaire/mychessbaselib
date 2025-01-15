#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <game.h>
#include <chessbase.h>
#include <dbasepool.h>
#include <QFile>
#include <QLibraryInfo>
#include <QDir>
#include <QFileDialog>
#include <scidbasemodel.h>
#include <QStandardItemModel>

static Game * scratchGame = NULL;   

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RemoveTestBase();
    connect ( ui->pusButtonCreateDataBaseFromPGNFIle,SIGNAL(clicked(bool)),this,SLOT(CreateDataBaseFromPGNFile()));
    connect ( ui->pushButtonQuit,SIGNAL(clicked(bool)),this,SLOT(close()));
    connect (ui->actionRemove_the_Test_DataBase,SIGNAL(triggered(bool)),this,SLOT(RemoveTestBase()));
    ScidBaseModel *scidbasemodel = new ScidBaseModel(this);
    // We pass to model  :
    //                       - the dir where are stored the file
    //                       - A QMap where we have the number of games for each opening
    scidbasemodel->setInfo(exampleDir(),mBaseInUse,ListScid5DataBase());
    ui->tableView->setModel(scidbasemodel);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createDir()
{
    QDir dir(exampleDir());
    if ( ! dir.exists())
        dir.mkdir(exampleDir());
}

QString MainWindow::exampleDir()
{
    return QDir::homePath()+"/scid5listTest/";
}

void MainWindow::showEvent(QShowEvent *event)
{
    qDebug()<<ui->tableView->width();
    ui->tableView->setColumnWidth(0,4*ui->tableView->width()/6);
    ui->tableView->setColumnWidth(1,ui->tableView->width()/6);
    ui->tableView->setColumnWidth(2,ui->tableView->width()/6);


}

void MainWindow::RemoveTestBase()
{
    QFile::remove(exampleDir()+"/test.si5");
    QFile::remove(exampleDir()+"/test.sg5");
    QFile::remove(exampleDir()+"/test.sn5");
}

void MainWindow::CreateDataBaseFromPGNFile()
{
    RemoveTestBase();
    QString filename=QFileDialog::getOpenFileName(this,tr("Open PGN file"),exampleDir(),tr("PGN files (*.pgn)"));
    QFileInfo fi(filename);
    DBasePool::init()  ;
    ChessBase cb(this,ui->progressBar);
    int numberbase;
    int code=cb.open(exampleDir()+"/"+fi.baseName(),ICodecDatabase::SCID5,FMODE_Create,numberbase);
    ui->labelError->setText(QString("Open %1 - %2").arg(cb.ErrorCode(code)).arg(numberbase));
    if ( code == OK) {
        scidBaseT *dbase =DBasePool::getBase(numberbase);
        int numberfound;
        code=cb.importGames(dbase,filename,numberfound);
        if ( code != OK)   ui->labelError->setText(QString("Import %1 - %2").arg(cb.ErrorCode(code)).arg(numberfound));
        else ui->labelError->setText("OK");
    }
}

QMap<QString, int> MainWindow::ListScid5DataBase()
{
    DBasePool::init()  ;
    int numberbase;

    ChessBase cb(this,ui->progressBar);
    QDir dir(exampleDir());
    QStringList filters; filters << "*.si5";
    QMap<QString, int> numbergames;

    for ( auto file : dir.entryList(filters,QDir::Files) ){
        QString file2=file.remove(".si5");
        int code=cb.open(exampleDir()+"/"+file,ICodecDatabase::SCID5,FMODE_Both,numberbase);
        mBaseInUse[file2]=true;
        if ( code == OK ) numbergames[file2]=cb.numberGames(DBasePool::getBase(numberbase));
        else  {
            numbergames[file2]=0;
            mBaseInUse[file2]=false;
        }
    }
    return numbergames;
}
