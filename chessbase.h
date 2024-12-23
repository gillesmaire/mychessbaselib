#ifndef CHESSBASE_H
#define CHESSBASE_H
#include <QObject>
#include "common.h"
#include "error.h"
#include "scidbase.h"
#include "progressbar.h"
#include <QList>
#include <QPair>

/// This Class is the Qt database management for Scid5 base with Qt and not with Tcl/Tk.
/// the sc_base.cpp has be converted with 

class ChessBase : public QObject
{
public:
    enum CommandFlag { nothing, get, set, unset, invert };
    
    ChessBase( QObject *ob);
    ///
    /// \brief open a base with value of codec  "MEMORY", "PGN", "SCID4","SCID5"
    /// \param filename : name of filename 
    /// \param codec : can be "MEMORY", "PGN", "SCID4","SCID5"
    /// \param fmode : can be member of enum FMODE_None = 0,FMODE_ReadOnly,FMODE_WriteOnly,FMODE_Both,FMODE_Create
    /// \param numberbase : a return parameter indicate the number of base selected
    /// \return :  0 if no error else a value given in error.h
    errorT open(QString filename, ICodecDatabase::Codec codec, fileModeT fmode , int  &numberbase);

    /// 
    /// \brief close the database given by the pointer
    /// \param pointer on Base
    /// \return  0 of not error else a value given by error.h 
    errorT close(scidBaseT *dbase);

    /// 
    /// \brief compact the database ie 
    ///     - remove all games marked deleted
    ///     - remove unused games 
    ///     - improve search operations 
    ///     - replace bad names by "???"
    /// \param pointer on base
    /// \return 0 of not error else a value given by error.h 
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
    /// \return 0 of not error else a value given by error.h 
    ///
    errorT statsInfoToBeCompacted(scidBaseT *dbase, QList<quint64> &list);
    
    ///
    /// \brief copyGames copy Game from dataBase to another one.
    /// \param sourcebase  is a pointer of source dataBase
    /// \param NumGameOrFilterName : can be a number of game or a FilterName 
    /// \param targedBaseId : Id of the target base 
    /// \return  0 of not error else a value given by error.h 
    ///
    errorT copyGames (scidBaseT *sourcebase, QString NumGameOrFilterName, uint  targedBaseId );
    
    ///
    /// \brief sc_base_create 
    /// \param name for the base
    /// \param codec MEMORY PGN SCID4 SCID5 
    /// \return 0 of not error else a value given by error.h
    ///
    errorT create(QString name,ICodecDatabase::Codec codec, int &numberBase  );
    
    ///
    /// \brief getExtraTags get le list of Pair of extratags 
    /// \param dbase pointer on the scidBaseT 
    /// \return  a QList of QPair<QString,QString>
    ///
    QList <QPair <QString,QString>>  getExtraTags(scidBaseT *dbase);
    
    ///
    /// \brief setExtraTags set in the Database extra tag value
    /// \param dbase : pointer on the base
    /// \param tag : tag name  to set
    /// \param value : tag value to set
    /// \return 
    ///
    errorT setExtraTags(scidBaseT *dbase , QString  tag, QString value);
    
    ///
    /// \brief filename returne the filename of database. 
    ///     - for PGN retule the full path filename 
    ///     - for scid base returne the name of base without the extention si4 or si5
    ///     - for clipbase return <clipbase>
    /// \param dbase pointer on the base 
    /// \return 
    ///
    QString filename( scidBaseT *dbase);
    ///
    /// \brief CodecEnum2Str convert Enum PGN SCID5 SCID4 MEM to QString equivalent
    /// \param codec the enum codec
    /// \param numberBase : the number of base
    /// \return the string in std::string format
    ///
    static std::string CodecEnum2Str( ICodecDatabase::Codec codec ) ;
    
    ///
    /// \brief CodecEnum2Str convert Enum PGN SCID5 SCID4 MEM to QString equivalent
    /// \param codec
    /// \return the string in QString format
    ///
     QString CodecEnum2QString( ICodecDatabase::Codec codec ) ;
    
    ///
    /// \brief getGameFlag get Flag flagtype of gamenum. For example D 
    /// \param dbase* : pointer on the base
    /// \param gamenum : game number
    /// \param flagtype : 'D' for Delete 'S' start see IndexEntry::CharToFlagMask
    /// \param result contain true if flag is true  or false else
    /// \return 0 of not error else a value given by error.h
    ///
    errorT getGameFlag( scidBaseT * dbase, uint gamenum, char flagtype, bool &result );
     
    ///
    /// \brief setGameFlag set a Flag for the game
    /// \param dbase * : pointer on base
    /// \param gamenum :  num of game to set he flag
    /// \param flagtype : the type fo flag to set 
    /// \return 0 of not error else a value given by error.h
    ///
    errorT setGameFlag( scidBaseT * dbase, uint gamenum, char flagtype );
    
    ///
    /// \brief unsetGameFlag unset a Flag for the game
    /// \param dbase * pointeur on base
    /// \param gamenum  :  num of game to set he flag
    /// \param flagtype : the type fo flag to set 
    /// \return 0 of not error else a value given by error.h
    ///
    errorT unsetGameFlag(scidBaseT *dbase, uint gamenum, char flagtype);
    
    ///
    /// \brief InvertGameFlag invert  a Flag for the game
    /// \param dbase * pointeur on base
    /// \param gamenum  :  num of game to invert he flag
    /// \param flagtype : the type fo flag to invert
    /// \return 0 of not error else a value given by error.h
    ///
    errorT InvertGameFlag(scidBaseT *dbase, uint gamenum, char flagtype);
    
    ///
    /// \brief gameCommandFlag : allows same operation like setGameFlag, getGameFlag, unsetGameFlag
    ///                          and invertGameFlag but allow to apply on gamenum but also on filtername
    ///                          or all 
    /// \param dbase * : pointer on base 
    /// \param filter : a string egal gameNum or filtername or all 
    /// \param command
    /// \param flagtype
    /// \return 
    ///
    errorT gameCommandFlag(scidBaseT *dbase, QString filter, CommandFlag command, char flagtype);
    
    
    ///
    /// \brief gamelocation find the position of a game by number of game
    /// \param dbase : pointer on scidBaseT 
    /// \param filt : a string egal gameNum or filtername or all
    /// \param sortCrit : the order for the list of games to search
    /// \param gnumber  : the number of game to search
    /// \param result : the position ( 0 for first ) if return is OK
    /// \return 0 for not error else a value given by error.h
    ///
    errorT gamelocation(scidBaseT *dbase, QString filt, QString sortCrit, uint gnumber, uint &result);
    
    errorT gamelocation(scidBaseT *dbase, QString filt, QString sort, QString text, uint start, bool forwardir, uint &result);
    
    errorT gameslist(scidBaseT *dbase, uint start, uint count, QString filt, QString sort, QList<QVariant> &res);

    static errorT getGameHelper(Game &game, QList<QVariant> &res);
    
    ///
    /// \brief getGame : return all positio of a game 
    /// \param dbase : pointer on scidBaseT
    /// \param gamenum: the number of requested game
    /// \param live :optional parameter which specify the behavior when the requested game have unsaved changes.
    ///              If present the latest unsaved version is returned, otherwise the saved copy in the database is used.
    /// \param res :  list containing all the positions of a game, including variations.
    /// The positions are sorted according to pgn standard:
    ///     The alternate move sequence given by an RAV is one that may be legally
    ///     played by first unplaying the move that appears immediately prior to the
    ///     RAV". For each position the following informations are provided: RAVdepth:
    ///     current variation depth. RAVnum: current variation num. FEN: "Forsyth-Edwards
    ///     Notation" describing the current position. NAGS: "Numeric Annotation Glyph" 
    ///     is a non-negative integer from 0 to 255 used to indicate a simple annotation
    ///     in a language independent manner. comment: text annotation of the
    ///     current position. lastMoveSAN: the last move that was played to reach the 
    ///     current position. The move is indicated using English "Standard Algebraic 
    ///  \return 0 for not error else a value given by error.h
    ///
    
    errorT getGame(scidBaseT *dbase, uint gamenum, bool live, QList<QVariant> &res);
};



#endif // CHESSBASE_H
