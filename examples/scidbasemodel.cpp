#include "scidbasemodel.h"
#include "chessbase.h"
#include "dbasepool.h"

#include  "mainwindow.h"
ScidBaseModel::ScidBaseModel(QWidget *parent)
    : QAbstractTableModel{parent}
{   DBasePool::init();
    mWid=parent;
}

void ScidBaseModel::setInfo(QString dir, uint number)
{
    mDir=dir;
    mNumber=number;

}

int ScidBaseModel::columnCount(const QModelIndex &parent) const{
    return 3;
}

int ScidBaseModel::rowCount(const QModelIndex &parent) const
{
    QStringList filters;
    filters << "*.si5";
    QDir d(mDir);
    return d.entryList(filters, QDir::Files).count();
}

QVariant ScidBaseModel::data(const QModelIndex &index, int role) const
{
    if (index.column()==0 )
    {   // name of database
        if (role == Qt::CheckStateRole) {
            return QVariant(); // no checkbox
        }
        QStringList filters;
        filters << "*.si5";
        QDir d(mDir);
        QRegularExpression regex("\\.si5$");
        QString res=d.entryList(filters, QDir::Files).at(index.row());
        res.replace(regex,"");
        return res;
    }
    else if (index.column()==1 )
    {
        // numbef of games in base
        if (role == Qt::CheckStateRole) {
            return QVariant(); // no checkbox
        }

        ChessBase cb(mWid);
        int nb;
        QString basename=index.sibling(index.row(),0).data().toString();
        QString file=MainWindow::exampleDir()+basename;
        errorT e=cb.open(file,ICodecDatabase::SCID5,FMODE_ReadOnly,nb);
        qDebug()<<e<<basename<<nb<<file;
        if (e!=OK) return 0;

        return nb;
    }
    else if (index.column()==2)
    {
        if (role == Qt::CheckStateRole) {
            return QVariant(); // no checkbox
        }
        return 0;
    }

}

QVariant ScidBaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if (role == Qt::DisplayRole) {
           if (orientation == Qt::Horizontal) {
               switch (section) {
                   case 0: return tr("Name");
                   case 1: return tr("Nb games");
                   case 2: return tr("Used");
                   default: return QVariant();
               }
            }
    }
    return QVariant();
}
