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
#include <QModelIndex>
#include <QString>
#include <QMessageBox>

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
    connect (ui->pushButtonTestDuplicate,SIGNAL(clicked(bool)),this,SLOT(TestDuplicate()));
    connect (ui->pushButtonCopyGame1Base1toBase2,SIGNAL(clicked(bool)),this,SLOT(CopyGame1Base1toBase2()));
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
    int numberbase;

    ChessBase cb(this,ui->progressBar);
    QDir dir(exampleDir());
    QStringList filters; filters << "*.si5";
    QMap<QString, int> numbergames;

    for ( auto file : dir.entryList(filters,QDir::Files) ){
        QString file2=file.remove(".si5");
        int code=cb.open(exampleDir()+file,ICodecDatabase::SCID5,FMODE_Both,numberbase);
        mBaseInUse[file2]=true;
        if ( code == OK ) numbergames[file2]=cb.numberGames(DBasePool::getBase(numberbase));
        else  {
            numbergames[file2]=0;
            mBaseInUse[file2]=false;
        }
    }
    return numbergames;
}


void MainWindow::TestDuplicate()
{

    if ( ! ui->tableView->selectionModel()->selectedIndexes().isEmpty()){
        QString selected=ui->tableView->selectionModel()->selectedIndexes().first().data().toString(); 
        QString searched=exampleDir()+selected+QString(".si5");
       int basehandle=DBasePool::find(searched.toStdString().c_str());
       scidBaseT *dbase= DBasePool::getBase(basehandle);
       ChessBase cb(this,ui->progressBar);
       if (cb.duplicates(dbase)== OK)  {
            QMessageBox::information(this,tr("Information"),QString("%1 database doesn't have duplicates games with default parameters").arg(selected));
          }
       else {
           QMessageBox::StandardButton rep=QMessageBox::question(this,tr("Question"),
           QString(tr("%1 database has dupplicates games and duplicate parameters have been marked whith to be delete flag. Do you want to compact now ? (yes/no)")));
           if (rep ==QMessageBox::Yes)  cb.compact(dbase);
       }
    }
}

void MainWindow::CopyGame1Base1toBase2()
{
    //before the size are 44621 8609 in my exampe
    int error=OK;
    
    QAbstractItemModel *model = ui->tableView->model();
    if (model && model->rowCount() >= 2  && model->columnCount() >= 2) {
        
    QModelIndex index1 = model->index(0, 0);
    QModelIndex index2 = model->index(1, 0);
    
    QString value1 =exampleDir()+ model->data(index1).toString()+".si5";
    int basehandle1=DBasePool::find(value1.toStdString().c_str());
    qDebug()<<value1;
    QString value2 =exampleDir()+ model->data(index2).toString()+".si5";
    int basehandle2=DBasePool::find(value2.toStdString().c_str());
    qDebug()<<value2;
    scidBaseT *dbase1= DBasePool::getBase(basehandle1);
    ChessBase cb(this,ui->progressBar);
    error=cb.copyGames(dbase1,1,basehandle2);
   }
   else 
    error=ERROR_BadArg;
   if ( error != OK ) 
     QMessageBox::information(this,tr("Information"),QString(tr("We cannot test copy from base 1 to base 2 with 0 or 1 base")));
   else 
     QMessageBox::information(this,tr("Information"),QString(tr("Copy of first game made nicely. You can check opening scid release compliant with scid5")));
}
