#include "chessbase.h"
#include "dbasepool.h"
#include "scidbase.h"
#include "progressbar.h"
#include "string_view"
#include "misc.h"

errorT ChessBase::open(QString codec, fileModeT fmode, QString filename, int &numberbase)
{
    if (DBasePool::find(filename.toStdString().c_str()) ) return (ERROR_FileInUse) ;
    auto dbase=DBasePool::getFreeSlot();
    if (!dbase ) return (ERROR_Full);
    errorT err=dbase->open(codec.toStdString().c_str(),fmode,filename.toStdString().c_str());
    mProgressBar=  new ProgressBar();
    if ( err != OK && err != ERROR_NameDataLoss ) return err;
    numberbase = DBasePool::switchCurrent(dbase);
    return err;
    }
    
errorT  ChessBase::close( scidBaseT *dbase)
{
    if (dbase->getFileName() == "<clipbase>") return  ERROR_BadArg;
	dbase->Close();
	delete (mProgressBar);
	return OK;
}

errorT ChessBase::compact(scidBaseT *dbase)
{

    mProgressBar = new ProgressBar();
    int err=dbase->compact(*mProgressBar);
    return  err;
}


errorT ChessBase::statsInfoToBeCompacted( scidBaseT *dbase,QList<quint64> &list)
{
    quint64 n_deleted, n_unused, n_sparse, n_badNameId;
	errorT res = dbase->getCompactStat(&n_deleted, &n_unused, &n_sparse, &n_badNameId);
	list.push_back(n_deleted);
	list.push_back(n_unused);
	list.push_back(n_sparse);
	list.push_back(n_badNameId);
	return  res;
}

errorT ChessBase::copyGames(scidBaseT *sourcebase, QString NumGameOrFilterName, uint targedBaseId)
{
    auto targetBase = DBasePool::getBase(targedBaseId);
    if ( targetBase == 0 ) return ERROR_BadArg; 
    errorT err=OK;
    
    const HFilter filter = sourcebase->getFilter(NumGameOrFilterName.toStdString());
    if ( filter != 0 ){
         mProgressBar=  new ProgressBar();
         err=targetBase->importGames(sourcebase, filter, *mProgressBar);
        }
    else {
         bool ok;
         uint gNum = NumGameOrFilterName.toUInt(&ok) ;
         const IndexEntry* ie = (gNum > 0)? sourcebase->getIndexEntry_bounds(gNum - 1): nullptr;
         if ( ie == nullptr ) return (ERROR_BadArg);
         Game game;
         err=sourcebase->getGame(*ie,game);
         if (err==OK )  err=targetBase->saveGame(&game);
    }
    return err;
}
