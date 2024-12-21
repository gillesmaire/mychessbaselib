#ifndef CHESSBASE_H
#define CHESSBASE_H
#include <QObject>
#include "common.h"
#include "error.h"
#include "scidbase.h"
#include "progressbar.h"

/// This Class is the Qt database management for Scid5 base with Qt and not with Tcl/Tk.
/// the sc_base.cpp has be converted with 

class ChessBase : public QObject
{
public:
    ChessBase( QObject *ob);
    ///
    /// \brief open a base with value of codec  "MEMORY", "PGN", "SCID4","SCID5"
    /// \param codec : can be "MEMORY", "PGN", "SCID4","SCID5"
    /// \param fmode : can be member of enum FMODE_None = 0,FMODE_ReadOnly,FMODE_WriteOnly,FMODE_Both,FMODE_Create
    /// \param filename : name of filename 
    /// \param numberbase : a return parameter indicate the number of base selected
    /// \return :  0 if no error else a value given in error.h
    errorT open( QString codec, fileModeT fmode ,QString filename, int  &numberbase);

    /// 
    /// \brief close the database given nhy the pointer
    /// \param pointer on Base
    /// \return  0 of not error else a value give by error.h 
    errorT close(scidBaseT *dbase);

    /// 
    /// \brief compact the database ie 
    ///     - remove all games marked deleted
    ///     - remove unused games 
    ///     - improve search operations 
    ///     - replace bad names by "???"
    /// \param pointer on base
    /// \return 0 of not error else a value give by error.h 
    ///
    errorT compact( scidBaseT *dbase) ;

    ///
    /// \brief stats return a QList of informations who must be compacted 
    /// \param dbase is a pointeur on the DataBase
    /// \param list is a QList of 4 informations :
    ///                     - games marked deleted
    ///                     - unused games
    ///                     - parse optimizations
    ///                     - bad named 
    /// \return 0 of not error else a value give by error.h 
    ///
    errorT statsInfoToBeCompacted(scidBaseT *dbase, QList<quint64> &list);
    
    ///
    /// \brief copyGames copy Game from dataBase to another one.
    /// \param sourcebase  is a pointer of source dataBase
    /// \param NumGameOrFilterName : can be a number of game or a FilterName 
    /// \param targedBaseId : Id of the target base 
    /// \return  0 of not error else a value give by error.h 
    ///
    errorT copyGames (scidBaseT *sourcebase, QString NumGameOrFilterName, uint  targedBaseId );
    
    
private:
   ProgressBar * mProgressBar;
};

#endif // CHESSBASE_H
