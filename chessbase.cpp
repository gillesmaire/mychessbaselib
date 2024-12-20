#include "chessbase.h"
#include "dbasepool.h"
#include "scidbase.h"

errorT ChessBase::close()
{
    
}

errorT ChessBase::open(QString codec, fileModeT fmode, QString filename, int &numberbase)
{
    if (DBasePool::find(filename.toStdString().c_str()) ) return (ERROR_FileInUse) ;
    auto dbase=DBasePool::getFreeSlot();
    if (!dbase ) return (ERROR_Full);
    errorT err=dbase->open(codec.toStdString().c_str(),fmode,filename.toStdString().c_str());
    // voir progress
    if ( err != OK && err != ERROR_NameDataLoss ) return err;
    numberbase = DBasePool::switchCurrent(dbase);
    return err;
    }
