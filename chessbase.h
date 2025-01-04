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

class ChessBase : public QWidget
{
  Q_OBJECT
private:

 struct dupCriteriaT {
    bool exactNames;
    bool sameColors;
    bool sameEvent;
    bool sameSite;
    bool sameRound;
    bool sameResult;
    bool sameYear;
    bool sameMonth;
    bool sameDay;
    bool sameEcoCode;
    bool sameMoves;
        };
    

struct gNumListT {
    uint64_t hash;
    uint gNumber;
    bool operator<(const gNumListT& a) const { return hash < a.hash; }
};

public:
    enum CommandFlag { nothing, get, set, unset, invert };
    enum deleteStrategyT { DELETE_SHORTER, DELETE_OLDER, DELETE_NEWER };
    
    
    
    
    
    ChessBase( QWidget *widget);
    ~ChessBase();
  
    ///
    /// \brief baseInUse 
    /// \param dbase
    /// \param number
    /// \return true if used false else

    bool baseInUse(int number );
      
      
    ///
    /// \brief Marked DEPRECATED
    ///         Change the current database and the current base 
    /// \param  dbase : pointer onto scidBaseT
    /// \return : the current Database ID after the switch
  
    int  BaseSwitch( scidBaseT *dbase);
    
    
    ///
    /// \brief checkDuplicate used by dupplicate check if two record games are dupplicated
    ///         The event, Site, Round, Year, Month, Day, Result, EcoCode and moves are tested
    ///         if Flag delete the record the result is false. 
    /// 
    /// \param dbase : pointer on scidBaseT
    /// \param ie1 : first element checked
    /// \param ie2 : second element checked
    /// \param cr :  citeria given by dupCriteriaT
    /// \return  true if the the game are equal fa
    ///
    bool checkDuplicate  (scidBaseT* dbase, const IndexEntry *ie1, const IndexEntry *ie2, dupCriteriaT *cr);
    
    ///
    /// \brief duplicates  : Finds duplicate games and marks them deleted.
    ///                      A pair of games are considered duplicates if the Event, Site,
    ///                      White, Black, and Round values all match identically, and the
    ///                      Date matches to within 2 days (that is, the same year, the 
    ///                      same month, and the days of month differ by 2 at most).
    /// 
    ///                      Furthermore, the moves of one game should, after truncating,
    ///                      be the same as the moves of the other game, for them to be 
    ///                      duplicate.
    /// 
    ///                      You can unset or set the criteria used /
    /// \param dbase : pointer on scidBaseT
    /// \param players : sometime they are error on name firstname default is false
    /// \param colors : generally true
    /// \param event  : generally true
    /// \param site   : generally true
    /// \param round  : generally true
    /// \param year   : generally true
    /// \param month  : generally true
    /// \param day    : generally false  begin or end of game ?
    /// \param result : genarally false (ir result is ometted)
    /// \param eco    : genrally false
    /// \param moves  : gnerally true
    /// \param skipShortGames
    /// \param keepAllCommentedGames
    /// \param keepAllGamesWithVars
    /// \param setFilterToDups
    /// \param onlyFilterGames
    /// \param Delete
    /// \return 
    ///
    errorT duplicates(scidBaseT *dbase, bool players = false, bool colors = true, bool event = true, bool site = true, bool round = true, bool year = true, bool month = true, bool day = false, bool result = false, bool eco = false, bool moves = true, bool skipShortGames = false, bool keepAllCommentedGames = true, bool keepAllGamesWithVars = true, bool setFilterToDups = false, bool onlyFilterGames = false, deleteStrategyT Delete = DELETE_SHORTER);
    
    
    /// \brief close the database given by the pointer
    /// \param pointer on Base
    /// \return  0 of not error else a value given by error.h 
    /// 
    errorT close(scidBaseT *dbase);
  
    
    /// \brief CodecEnum2Str convert Enum PGN SCID5 SCID4 MEM to QString equivalent
    /// \param codec
    /// \return the string in QString format
    ///
    QString CodecEnum2QString( ICodecDatabase::Codec codec ) ;
    
    ///
    /// \brief CodecEnum2Str convert Enum PGN SCID5 SCID4 MEM to string equivalent
    /// \param codec the enum codec
    /// \param numberBase : the number of base
    /// \return the string in std::string format
    
    static std::string CodecEnum2Str( ICodecDatabase::Codec codec ) ;
    
    
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
    /// \brief copyGames copy Game from dataBase to another one.
    /// \param sourcebase  is a pointer of source dataBase
    /// \param NumGameOrFilterName : can be a number of game or a FilterName 
    /// \param targedBaseId : Id of the target base 
    /// \return  0 of not error else a value given by error.h 
    ///
    errorT copyGames (scidBaseT *sourcebase, QString NumGameOrFilterName, uint  targedBaseId );
  
    /// \brief create a new database (is the convenience method for open with flag create)
    ///        the Database created is empty.
    /// \param name for the base.
    ///     - if codec is SCID4 create the files name.si4 name.sg4 name.sn4 
    ///     - if codec is SCID5 create the files name.si5 name.sg5 name.sn5 
    ///     - if codec is PGN create the file  name.pgn. If pgn extention is ometted the
    ///       file PGN is created but without extention .pgn
    ///     - if name = /dir/name create the file in the directory /dir/
    /// \param codec MEMORY PGN SCID4 SCID5 
    /// \param numberBase : is the number of base used (you can use 9 bases at the same time)
    /// \return 0 of not error else a value given by error.h

    errorT create(QString name,ICodecDatabase::Codec codec, int &numberBase  );
    
    ///
    /// \brief ErrorCode get a QString from errorT code 
    /// \param code : the code returned by a function 
    /// \return the QString containing 
    ///                 - the ERROR CODE in integer value (0)
    ///                 - the ERROR CODE in QString value (OK)
    ///
    QString ErrorCode( errorT code ) ;
    
    ///
    /// \brief exportBase
    /// \param dbase
    /// \param filter
    /// \param appendMode
    /// \param outputFormat
    /// \param outputFile
    /// \param append
    /// \param starttext
    /// \param endtext
    /// \param comments
    /// \param variation
    /// \param space
    /// \param symbols
    /// \param indentComments
    /// \param indentVariation
    /// \param column
    /// \param noMakeCodes
    /// \param convertNullMoves
    /// \return 
    ///
    
    int exportBase(const scidBaseT *dbase, QString filter, bool appendToFile, gameFormatT outputFormat, QString exportFileName, QString startText=QString(), QString endText=QString(), bool comments=false, bool variation=false, bool spaces=false, bool symbols=false, bool indentComments=false, bool indentVariation=false, bool column=false, bool noMarkCodes=false, bool convertNullMoves=false);
 
    void exportGame(Game *g, FILE *exportFile, gameFormatT format, uint pgnStyle );
    
    ///
    /// \brief piecetrack  : Examines games in the filter of the current database and return a list of 64 integers indicating
    ///        how frequently the specified piece moves for each square
    /// \param dbase : pointer on the base 
    /// \param timeOnSquareMode : by default the timeOnSquareMode is false and gameOnSquareMode is set.
    ///        on timeOnSquareMode the mode is given in time , on gameonSquareMode the mode is give in number of game
    /// \param minMoves : anolyses from minMoves for example 0 for start of games 10 : after 10 moves 
    /// \param maxMoves : analyses  to maxMoves  for example 30 to analyse  until 30 moves
    /// \param moves : list of case to analyse for example "e4 d4"
    /// \param arr : the result is array ok 64 uint declared as reference
    /// \return OK for not error else a value given by error.h
    ///
    
    
    ///
    /// \brief filename return the filename of database. 
    ///     - for PGN returns the full path filename 
    ///     - for scid base returns the name of base without the extention si4 or si5
    ///     - for clipbase returns <clipbase>
    /// \param dbase pointer on the base 
    /// \return 
    ///
    /// 
   
    QString filename( scidBaseT *dbase);

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
    /// \brief gameslist
    /// \param dbase
    /// \param start
    /// \param count
    /// \param filt
    /// \param sort
    /// \param res
    /// \return 
    ///
    errorT gameslist(scidBaseT *dbase, uint start, uint count, QString filt, QString sort, QList<QVariant> &res);
    
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
    
    ///
    /// \brief gamelocation
    /// \param dbase
    /// \param filt
    /// \param sort
    /// \param text
    /// \param start
    /// \param forwardir
    /// \param result
    /// \return 
    ///
    errorT gamelocation(scidBaseT *dbase, QString filt, QString sort, QString text, uint start, bool forwardir, uint &result);



    ///
    /// \brief gamesummary
    /// \param base
    /// \param gameNum
    /// \param res
    /// \return 
    ///   
    errorT gamesummary(const scidBaseT &base, uint gameNum, QList<QVariant> &res);
    
    
    ///
    /// \brief getExtraTags get le list of Pair of extratags 
    /// \param dbase pointer on the scidBaseT 
    /// \return  a QList of QPair<QString,QString>
    ///
    
    QList <QPair <QString,QString>>  getExtraTags(scidBaseT *dbase);
    
        
    ///
    /// \brief getGame : return all positions of a game 
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
    
    
    ///
    /// \brief getGameFlag
    /// \param dbase
    /// \param gamenum
    /// \param flagtype
    /// \param result
    /// \return 
    ///
    errorT getGameFlag( scidBaseT * dbase, uint gamenum, char flagtype, bool &result );
     
    ///
    /// \brief unsetGameFlag unset a Flag for the game
    /// \param dbase * pointeur on base
    /// \param gamenum  :  num of game to set he flag
    /// \param flagtype : the type fo flag to set 
    /// \return 0 of not error else a value given by error.h
    ///
     
    /// \brief open a base with value of codec  "MEMORY", "PGN", "SCID4","SCID5"
    /// \param filename : name of filename 
    /// \param codec : can be "MEMORY", "PGN", "SCID4","SCID5"
    /// \param fmode : can be member of enum FMODE_None = 0,FMODE_ReadOnly,FMODE_WriteOnly,FMODE_Both,FMODE_Create
    /// \param numberbase : a return parameter indicate the number of base selected
    /// \return :  0 if no error else a value given in error.h
    errorT open(QString filename, ICodecDatabase::Codec codec, fileModeT fmode , int  &numberbase);
    
    ///
    /// \brief open
    /// \param dbase
    /// \param codec
    /// \param filename
    /// \return 
    ///
    uint open(scidBaseT *dbase, ICodecDatabase::Codec codec, QString filename);
    ///
    /// \brief setGameFlag set a Flag for the game
    /// \param dbase * : pointer on base
    /// \param gamenum :  num of game to set he flag
    /// \param flagtype : the type fo flag to set 
    /// \return 0 of not error else a value given by error.h
    ///
    errorT setGameFlag( scidBaseT * dbase, uint gamenum, char flagtype );
    
    ///
    /// \brief setExtraTags set in the Database extra tag value
    /// \param dbase : pointer on the base
    /// \param tag : tag name  to set
    /// \param value : tag value to set
    /// \return 
    ///
    errorT setExtraTags(scidBaseT *dbase , QString  tag, QString value);
  
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
    /// \brief getGameFlag get Flag flagtype of gamenum. For example D 
    /// \param dbase* : pointer on the base
    /// \param gamenum : game number
    /// \param flagtype : 'D' for Delete 'S' start see IndexEntry::CharToFlagMask
    /// \param result contain true if flag is true  or false else
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
    
    static errorT getGameHelper(Game &game, QList<QVariant> &res);
    
    /// \brief importGames import  a PGN game in base
    /// \param dbase : * pointeur on base 
    /// \param fileName : the path and pgn file name to import for example :
    ///                     QString("/home/test/PGN/00003265gamses.pgn") or
    ///                     "/home/test/PGN/00003265gamses.pgn"
    /// \param numgame : number of game imported returned  ImportGames
    /// \return  0 of not error else a value given by error.h
    ///
    errorT importGames(scidBaseT *dbase, QString fileName, int &numgame);

    
    ///
    /// \brief numberGames return the number of games in a dataBase
    /// \param dbase the pointer on the database
    /// \return number of games in uint
    ///
    uint numberGames(scidBaseT *dbase);
    
    ///
    /// \brief ListOpenedBases
    /// \param dbase
    /// \return 
    ///
    uint ListOpenedBases(scidBaseT *dbase);
    
    
    ///
    ////// \brief numberGames
    ////// \param dbase
    ////// \param baseId
    ////// \return 
    ///
    
    uint numberGames(scidBaseT *dbase, int baseId);
    
    ///
    /// \brief piecetrack
    /// \param dbase
    /// \param timeOnSquareMode
    /// \param minMoves
    /// \param maxMoves
    /// \param moves
    /// \param result
    /// \return 
    ///
    
    errorT piecetrack(scidBaseT *dbase, bool timeOnSquareMode,  uint minMoves, uint maxMoves, QString moves, std::array<unsigned int, 64>& result);
   
    
    ///
    /// \brief playerElo return a list of elo values of a player
    /// \param dbase : pointer on Scid Base
    /// \param filterName : 
    /// \param playerName
    /// \param res
    /// \return OK for not error else a value given by error.h
    ///
    
    errorT playerElo(const scidBaseT *dbase, QString filterName, QString playerName, QList<QPair<unsigned int, ushort> > &res);
  
    ///
    /// \brief Slot get the handle of opened database
    /// \param filename : the filename of wanted database 
    ///                   Database in native Scid format doesn't use extension 
    ///                   Other databases require file extention, (.pgn)
    /// \return the handke of the database corresponding to fileName
    ///
    int Slot( QString fileName);
    
    ///
    /// \brief sortChacheCreate create a sortCache
    /// \param dbase : pointer on Base
    /// \param sortCrit : Sort critera
    /// \return  : 0 for not error else a value given by error.h
    ///
    errorT sortChacheCreate( scidBaseT *dbase,  QString sortCrit );
    
    ///
    /// \brief sortChacheRelease 
    /// \param dbase : pointer on Base
    /// \param sortCrit :  sort critera
    /// 
    ///
    void sortChacheRelease( scidBaseT *dbase, QString sortCrit );
    
    ///
    /// \brief statsDates return statistic about minimum dates, maximum dates and mean year 
    /// \param dbase : pointer on Base
    /// \return  QList of 3 uint32-t
    ///
    QList<quint32> statsDates (scidBaseT *dbase ) ;
    
    ///
    /// \brief statsECO return statistic about 
    ///          - number of games with the ECO
    ///          - number of white wins
    ///          - number of draws
    ///          - number of black wins
    ///          - number of no result games
    ///          - score in %
    /// \param dbase : pointer on Base
    /// \param ECO  : must be a valid ECO code or an abbreviation.
    ///  For example:
    ///       A returns the sum of all the A00,A00a...A99z4
    ///       A1 returns the sum of all the A10,A10a,A10a1...A19z4
    ///       A10 returns the sum of all the A10,A10a,A10a1...A10z4
    ///       An empty string returns the sum of all valid ECO codes
    /// \return  a QList of 5 results
    ///
    QList<quint32> statsECO (scidBaseT *dbase , QString ECO ) ;
    
    ///
    /// \brief statsFLAG : number of  games with the flag set
    ///             The flag must be a valid flag char like in
    ///             IndexEntry::CharToFlag() ie 
    ///         D   DELETE 
    ///         W   WHITE OP
    ///         B   Black OP
    ///         M   Middlegame 
    ///         E   EndGame
    ///         N   Novelty
    ///         P   Pawn
    ///         T   TACTICS
    ///         K   KSIDE
    ///         Q   QSIDE 
    ///         !   BRILLANCY
    ///         ?   BLUNDER
    ///         U   User
    ///         1..6 CUSTOM
    /// \param dbase : pointer on Base
    /// \param flag : the flag
    /// \return 
    ///
    quint32 statsFLAG (scidBaseT *dbase, QChar flag) ;
    
    ///
    /// \brief statsFLAGS number of  games with the flags D W and B set
    /// \param dbase : pointer on Base
    /// \return 
    ///
    QList<quint32> statsFLAGS(scidBaseT *dbase);

    ////
    /// \brief statsRATINGS
    /// \param dbase : pointer on Base
    /// \return {minimum elo} {maximum elo} {mean elo}
    ///
    QList<quint32> statsRATINGS(scidBaseT *dbase);
    
    ///
    /// \brief statsRESULTS
    /// \param dbase : pointer on Base
    /// \return  {number of games won by white} {number of draws} {number of
    ///         games won by black} {number of games with no result}
    ///
    QList<quint32> statsRESULTS(scidBaseT *dbase);
    
  
    
    ///
    /// \brief strip remove all occurences of the spedified tags from the database
    /// \param dbase : pointer onto scidBaseT
    /// \param tagNames : list on tag names 
    /// \return 
    ///
    QList<unsigned short> strip(scidBaseT *dbase, QStringList tagNames);
    
    
    ///
    /// \brief tagList produce a list of PGN tags used in the DataBase 
    /// \param dbase : pointer onto scidBaseT
    /// \param QList<QPair<QString,int>> &res output paramter if errorT is OK
    /// \return OK for not error else a value given by error.h
    ///
    errorT tagList(scidBaseT *dbase,QList<QPair<QString,int>> &res);
    
    ///
    /// \brief tournaments return a list of tournaments
    ///         Games with the same Event Site and EventDate tags are considered as 
    ///         a tournament
    ///         The list returned can be restricted based on :
    ///                - the average elo og participants
    ///                - the number of games 
    ///                - the number of players 
    ///                - the nae of a participant
    ///         Ranges can be specified with one or two numvers separated by spaces (min max)
    ///         The returned  list cat ne sorted according to the followinf critera : 
    ///                - Date  :  date on the first games in the tournament
    ///                - Players : number of participants to tournament
    ///                - Games : number of games
    ///                - Elo  : average elo of the participants
    ///                - Site : site name 
    ///                - Event : event name
    /// \param dbase    
    /// \param maxresult
    /// \return on Succes return a list of tournaments 
    ///         ofr eah tournament the followinf informations are provided :
    ///      date of the first game, site name, event name, number of players , number of games, average elo, lowest game number,
    ///     winner name, winner elo, winner score, 2nd place name , 2 nd elo 2nd score
    /// 
 
    errorT tournaments( const scidBaseT * dbase, Filter, QString filter, long maxresult, QList<QList<QVariant>> &res, int elo =0, 
                        int games=0, int players=0, QString player=QString(),  QString sort= QString());
    
};

#endif // CHESSBASE_H
