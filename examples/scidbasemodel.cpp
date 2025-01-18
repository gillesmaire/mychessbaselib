#include "scidbasemodel.h"
#include "chessbase.h"
#include "dbasepool.h"

#include  "mainwindow.h"
ScidBaseModel::ScidBaseModel(QWidget *parent)
    : QAbstractTableModel{parent}
{   
    mWid=parent;
}

void ScidBaseModel::setInfo(QString dir,QMap<QString,bool> baseused,QMap<QString, int> listnumber)
{
    mDir=dir;
    mListNumberGames=listnumber;
    mBasesInUse=baseused;
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
        // number of games in base
        if (role == Qt::CheckStateRole) {
            return QVariant(); // no checkbox
        }
        //mListNumberGames[index.sibling(index.row(),0).data().toString()];
        return mListNumberGames[index.sibling(index.row(),0).data().toString()];
    }
    else if (index.column()==2)
    {
        if (role == Qt::CheckStateRole) {
            return QVariant(); // no checkbox
        }
        return (mBasesInUse[index.sibling(index.row(),0).data().toString()]);
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
