#include "chessbase.h"
#include "dbasepool.h"
#include "scidbase.h"
#include "progressbar.h"
#include "misc.h"
#include "dstring.h"
#include "searchtournaments.h"
#include <QList>
#include <QPair>
#include <filesystem>
#include <date.h>
#include <QMap>
#include <filebuf.h>
#include <cstdio>



static Game *scratchGame = NULL;

errorT ChessBase::open(QString filename,ICodecDatabase::Codec codec, fileModeT fmode,  int &numberbase)
{
    if (DBasePool::find(filename.toStdString().c_str()) ) return (ERROR_FileInUse) ;
    auto dbase=DBasePool::getFreeSlot();
    if (!dbase ) return (ERROR_Full);
    ProgressBar pb;
    errorT err=dbase->open(CodecEnum2Str(codec),fmode,filename.toStdString().c_str(),pb);
    if ((err == ERROR_FileOpen || err == ERROR_FileMode) &&
	    fmode == FMODE_Both)
	    err = dbase->open(CodecEnum2Str(codec), FMODE_ReadOnly, filename.toStdString().c_str(), pb);
    if ( err != OK && err != ERROR_NameDataLoss ) return err;
    numberbase = DBasePool::switchCurrent(dbase);
    return err;
    }
    
errorT  ChessBase::close( scidBaseT *dbase)
{
    if (dbase->getFileName() == "<clipbase>") return  ERROR_BadArg;
	dbase->Close();
	return OK;
}

errorT ChessBase::compact(scidBaseT *dbase)
{
    ProgressBar pb;
    int err=dbase->compact(pb);
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
         ProgressBar pb;
         err=targetBase->importGames(sourcebase, filter,pb);
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

errorT ChessBase::create(QString name ,ICodecDatabase::Codec codec, int &numberbase)
{
    return open(name,codec,FMODE_Create,numberbase);
}

QList<QPair<QString, QString> > ChessBase::getExtraTags(scidBaseT *dbase)
{   
    auto list = dbase->getExtraInfo();
    QList <QPair<QString, QString>> result;
    for (const auto& e : list)
    {
        QPair<QString, QString> p(QString(e.first),QString(e.second.c_str()));
        result<<p;
    }
    return result;
}

errorT ChessBase::setExtraTags(scidBaseT *dbase,QString tag, QString value)
{
   return dbase->setExtraInfo(tag.toStdString().c_str(),value.toStdString().c_str()) ;
}

QString ChessBase::filename(scidBaseT *dbase)
{
    return QString(dbase->getFileName().c_str());
}


std::string ChessBase::CodecEnum2Str(ICodecDatabase::Codec codec)
{
    if ( codec==ICodecDatabase::MEMORY ) return ("Memory");
    else if (codec==ICodecDatabase::PGN ) return ("PNG");
    else if (codec==ICodecDatabase::SCID4 ) return ("SCID4");
    else  return ("SCID5");
}

QString ChessBase::CodecEnum2QString(ICodecDatabase::Codec codec)
{   
    std::string st=CodecEnum2Str(codec);
    return (QString (st.c_str()));
}

errorT ChessBase::getGameFlag(scidBaseT *dbase, uint gamenum, char flagtype , bool &result)
{

   uint flag = IndexEntry::CharToFlagMask(flagtype);
   if ( gamenum > 0 && gamenum < dbase->numGames())
        result = dbase->getFlag(flag,gamenum-1);
   else return  ERROR_NumGamesLimit ;
   return OK;
  
}

errorT ChessBase::setGameFlag(scidBaseT *dbase, uint gamenum, char flagtype)
{
    uint flag = IndexEntry::CharToFlagMask(flagtype);
    if ( gamenum > 0 && gamenum < dbase->numGames())
       dbase->setFlag(true,flag,gamenum-1);
    else return  ERROR_NumGamesLimit ;
    return OK;
}

errorT ChessBase::unsetGameFlag(scidBaseT *dbase, uint gamenum, char flagtype)
{
    uint flag = IndexEntry::CharToFlagMask(flagtype);
    if ( gamenum > 0 && gamenum < dbase->numGames())
       dbase->setFlag(false,flag,gamenum-1);
    else return  ERROR_NumGamesLimit ;
    return OK;
}

errorT ChessBase::InvertGameFlag(scidBaseT *dbase, uint gamenum, char flagtype)
{
    uint flag = IndexEntry::CharToFlagMask(flagtype);
    if ( gamenum > 0 && gamenum < dbase->numGames())
       dbase->invertFlag(flag,gamenum-1);
    else return  ERROR_NumGamesLimit ;
    return OK;
}

errorT ChessBase::gameCommandFlag(scidBaseT *dbase, QString filt, CommandFlag command, char flagtype)
{
    uint flagType = IndexEntry::CharToFlagMask(flagtype);
	if (flagType != 0 && command != CommandFlag::nothing )  {
		const HFilter filter = dbase->getFilter(filt.toStdString().c_str());
		if (filter != 0) {
			switch (command) {
			case set:  dbase->setFlags(true, flagType, filter); return OK;
			case unset:  dbase->setFlags(false, flagType, filter);return OK;
			case invert: dbase->invertFlags(flagType, filter);return OK;
			case nothing: ;
			case get : ;
			}
		} else {
			gamenumT gNum = strGetUnsigned(filt.toStdString().c_str());
			if (gNum > 0 && gNum <= dbase->numGames()) {
				gNum--;
				switch (command) {
				case 1: dbase->getFlag(flagType, gNum); return OK;
				case 2: dbase->setFlag(true, flagType, gNum) ; return OK;
				case 3: dbase->setFlag(false, flagType, gNum);return OK;
				case 4: dbase->invertFlag(flagType, gNum); return OK;
				case nothing : return ERROR_NumGamesLimit;
				}
			}
		}
	}  
	return ERROR_NumGamesLimit;
}


errorT ChessBase::gamelocation(scidBaseT *dbase, QString filt, QString sortCrit, uint gnumber , uint &result)
{
    const HFilter filter = dbase->getFilter(filt.toStdString().c_str());
    if (filter == 0) return ERROR_BadArg;
    result = dbase->sortedPosition(sortCrit.toStdString().c_str(), filter, gnumber - 1);
	if (result == INVALID_GAMEID)
		return ERROR_InvalidFEN ;
	return  OK; 
   
}


///
/// \brief gamelocation find the first game that contains text in white name or black name of  event name or site name 
/// \param dbase : pointer on base
/// \param filt :  a string egal gameNum or filtername or all
/// \param sort : the order for the list of games to search
/// \param text : text of white name nlack name or whit or black event or site
/// \param start  :  offset to begining to search 
/// \param forwardir : if true search from startpos tp num game else search from 0 to num game
/// \param result the position ( 0 for first ) if return is OK  
/// \return 0 for not error else a value given by error.h
///
errorT ChessBase::gamelocation (scidBaseT *dbase, QString filt, QString sort, QString text, uint start, bool forwardir , uint &result)
{
   const HFilter filter = dbase->getFilter(filt.toStdString().c_str());
   if (filter == 0) return ERROR_BadArg;
   size_t location = INVALID_GAMEID;
   auto contains = [&](gamenumT g) {
			const auto tags = dbase->tagRoster(g);
			return strAlphaContains(tags.white, text.toStdString().c_str()) ||
			       strAlphaContains(tags.black, text.toStdString().c_str()) ||
			       strAlphaContains(tags.event, text.toStdString().c_str()) ||
			       strAlphaContains(tags.site, text.toStdString().c_str());
		};
    if (forwardir)  {
            std::vector<gamenumT> buf(
			    start >= filter->size() ? 1 : filter->size() - start);
			buf.resize(
			    dbase->listGames(sort.toStdString().c_str(), start, buf.size(), filter, buf.data()));
			auto it = std::find_if(buf.begin(), buf.end(), contains);
			if (it != buf.end())
				location = start + std::distance(buf.begin(), it);
    }
    else {
    std::vector<gamenumT> buf(start ? start : 1);
			buf.resize(dbase->listGames(sort.toStdString().c_str(), 0, start, filter, buf.data()));
			auto it = std::find_if(buf.rbegin(), buf.rend(), contains);
			if (it != buf.rend())
				location = std::distance(it, buf.rend()) - 1;
    }
    if (location == INVALID_GAMEID)
		return ERROR_InvalidFEN ;
    result=location;
	return  OK; 
}


errorT ChessBase::gameslist(scidBaseT* dbase, uint start, uint  count, QString filt, QString sort , QList<QVariant>  &res)
{
   

	const HFilter filter = dbase->getFilter(filt.toStdString().c_str());
	if (filter == NULL)
		return  ERROR_BadArg;
	gamenumT* idxList = new gamenumT[count];
	count = dbase->listGames(sort.toStdString().c_str(), start, count, filter, idxList);

//	QList<QVariant> res (count * 3);
	QList<QVariant> ginfo(24);
	for (uint i = 0; i < count; ++i) {
		uint idx = idxList[i];

		ASSERT(filter->get(idx) != 0);
		uint ply = filter->get(idx) -1;

		const IndexEntry* ie = dbase->getIndexEntry(idx);
		const auto tags = dbase->tagRoster(*ie);

		ginfo.clear();
		ginfo.push_back(idx +1);
		ginfo.push_back(RESULT_STR[ie->GetResult()]);
		ginfo.push_back((ie->GetNumHalfMoves() + 1) / 2);
		ginfo.push_back(tags.white);
		std::string eloStr;
		eloT welo = ie->GetWhiteElo();
		if (welo != 0) {
			eloStr = std::to_string(welo);
		} else {
			welo = dbase->peakElo(ie->GetWhite());
			eloStr = std::to_string(welo);
			if (welo != 0) {
				eloStr.insert(eloStr.begin(), '(');
				eloStr.insert(eloStr.end(), ')');
			}
		}
		
		ginfo.push_back(QString::fromUtf8(eloStr));
		ginfo.push_back(tags.black);
		eloT belo = ie->GetBlackElo();
		if (belo != 0) {
			eloStr = std::to_string(belo);
		} else {
			belo = dbase->peakElo(ie->GetBlack());
			eloStr = std::to_string(belo);
			if (belo != 0) {
				eloStr.insert(eloStr.begin(), '(');
				eloStr.insert(eloStr.end(), ')');
			}
		}
		ginfo.push_back(QString::fromUtf8(eloStr));
		char buf_date[16];
		date_DecodeToString(ie->GetDate(), buf_date);
		ginfo.push_back(QString(buf_date));
		ginfo.push_back(tags.event);
		ginfo.push_back(tags.round);
		ginfo.push_back(tags.site);
		ginfo.push_back(ie->GetNagCount());
		ginfo.push_back(ie->GetCommentCount());
		ginfo.push_back(ie->GetVariationCount());
		char deleted[2] = {0};
		deleted[0] = (ie->GetDeleteFlag()) ? 'D' : ' ';
		ginfo.push_back(QString(deleted));
		char flags[16];
		ie->GetFlagStr (flags, "WBMENPTKQ!?U123456");
		ginfo.push_back(QString(flags));
		ecoStringT ecoStr;
		eco_ToExtendedString (ie->GetEcoCode(), ecoStr);
		ginfo.push_back(QString(ecoStr));
		std::string endMaterial = matsig_makeString(ie->GetFinalMatSig());
		ginfo.push_back(QString::fromStdString(endMaterial));
		char startpos[2] = {0};
		startpos[0] = (ie->GetStartFlag()) ? 'S' : ' ';
		ginfo.push_back(QString(startpos));
		char buf_eventdate[16];
		date_DecodeToString (ie->GetEventDate(), buf_eventdate);
		ginfo.push_back(QString(buf_eventdate));
		ginfo.push_back(ie->GetYear());
		ginfo.push_back((welo + belo)/2);
		ginfo.push_back(ie->GetRating());
		ginfo.push_back(QString::fromUtf8(dbase->getGame(ie).getMoveSAN(ply, 10)));

		res.push_back(QString::fromUtf8(std::to_string(idx+1) + "_" + std::to_string(ply)));
		res.push_back(ginfo);
		res.push_back(QString(deleted));
	}

	delete [] idxList;
	return  OK;
}


errorT  ChessBase::getGameHelper( Game& game , QList<QVariant>  &res) {
auto positions = gamepos::collectPositions(game);
	QList<QVariant> posInfo;
	for (const auto& pos : positions) {
		posInfo.clear();
		posInfo.push_back(pos.RAVdepth);
		posInfo.push_back(pos.RAVnum);
		posInfo.push_back(QString::fromUtf8(pos.FEN));
		std::string nags;
		for (const auto& nag : pos.NAGs) {
			char temp[16];
			game_printNag(nag, temp, true, PGN_FORMAT_Plain);
			if (!nags.empty())
				nags += ' ';
			nags += temp;
		}
		posInfo.push_back(QString::fromUtf8(nags));
		posInfo.push_back(QString::fromUtf8(pos.comment));
		posInfo.push_back(QString::fromUtf8(pos.lastMoveSAN));
		res.push_back(posInfo);
	}
	return  OK;
}


errorT ChessBase::getGame( scidBaseT *dbase, uint gamenum, bool live , QList<QVariant> &res)
{
	gamenumT gNum = gamenum;
	if (live && dbase->gameNumber == (static_cast<long long>(gNum) - 1)) {
		auto location = dbase->game->currentLocation();
		int ret=getGameHelper( *(dbase->game), res);
		if (ret != OK) return ret;
		dbase->game->restoreLocation(location);
		
	}
	auto ie = (gNum > 0) ? dbase->getIndexEntry_bounds(gNum - 1) : nullptr;
	if (!ie)
		return  ERROR_BadArg;

	Game game;
	errorT err = dbase->getGame(*ie, game);
	if (err != OK) return err; 
	return getGameHelper(game, res);
}

errorT ChessBase::importGames(scidBaseT* dbase, QString fileName, int &numgame)
{

	auto nImported = dbase->numGames();
	std::string errorMsg; 
	ProgressBar pb;
	if (auto err = dbase->importGames(ICodecDatabase::PGN,fileName.toStdString().c_str(),pb,errorMsg) )
		return  err;
	numgame=dbase->numGames() - nImported;
	return OK;
}


uint ChessBase::ListOpenedBases( scidBaseT *dbase)
{
    return dbase->numGames();
}

int ChessBase::Slot(QString fileName)
{
   return( DBasePool::find(fileName.toStdString().c_str())) ;
}

errorT ChessBase::sortChacheCreate(scidBaseT *dbase, QString sortCrit)
{
  if  ( ! dbase->createSortCache( sortCrit.toStdString().c_str()) )
     return ERROR;
  return OK;
}

void ChessBase::sortChacheRelease(scidBaseT *dbase, QString sortCrit)
{
   dbase->releaseSortCache( sortCrit.toStdString().c_str()) ;
}

QList<quint32> ChessBase::statsDates(scidBaseT *dbase)
{
    QList<quint32> res;
    auto const& stats = dbase->getStats();
    res.push_back(date_GetYear(stats.minDate));
    res.push_back(date_GetYear(stats.maxDate));
    res.push_back(stats.nYears == 0 ? 0 : size_t(stats.sumYears / stats.nYears));
    return res;
}

QList<quint32> ChessBase::statsECO(scidBaseT *dbase, QString ECO)
{
    QList<quint32> res;
    auto const& stats = dbase->getStats();
    
    const scidBaseT::Stats::Eco* eco = stats.getEcoStats(ECO.toStdString().c_str());
    res.push_back(eco->count);
    res.push_back(eco->results[RESULT_White]);
	res.push_back(eco->results[RESULT_Draw]);
	res.push_back(eco->results[RESULT_Black]);
	res.push_back(eco->results[RESULT_None]);
	uint count = eco->count - eco->results[RESULT_None];
	uint score = eco->results[RESULT_White] * 2;
	score += eco->results[RESULT_Draw];
	score *= 500;
	res.push_back(count == 0 ? 0.0 : score / count / 10.0);
	return res;
}

quint32 ChessBase::statsFLAG(scidBaseT *dbase, QChar flag)
{
    auto const& stats = dbase->getStats();
    quint32 Flag = IndexEntry::CharToFlag(flag.toLatin1());
    return stats.flagCount[Flag];
}

QList <quint32> ChessBase::statsFLAGS(scidBaseT *dbase)
{
    QList<quint32> res;
    auto const& stats = dbase->getStats();
    res.push_back(stats.flagCount[IndexEntry::CharToFlag('D')]);
	res.push_back(stats.flagCount[IndexEntry::CharToFlag('W')]);
	res.push_back(stats.flagCount[IndexEntry::CharToFlag('B')]);
    return res;
}

QList<quint32> ChessBase::statsRATINGS(scidBaseT *dbase)
{
    QList<quint32> res;
    auto const& stats = dbase->getStats();
    res.push_back(stats.minRating);
	res.push_back(stats.maxRating);
	res.push_back(stats.nRatings == 0 ? 0 : size_t(stats.sumRatings / stats.nRatings));  
	return res;
}

QList<quint32> ChessBase::statsRESULTS(scidBaseT *dbase)
{
    QList<quint32> res;
    auto const& stats = dbase->getStats();
    res.push_back(stats.nResults[RESULT_White]);
    res.push_back(stats.nResults[RESULT_Draw]);
	res.push_back(stats.nResults[RESULT_Black]);
	res.push_back(stats.nResults[RESULT_None]);
	return res;
}

int ChessBase::BaseSwitch(scidBaseT *dbase)
{
    return DBasePool::switchCurrent(dbase);
}




QList<unsigned short> ChessBase::strip(scidBaseT *dbase, QStringList tagNames)
{
    Filter filter_all(dbase->numGames());
    ProgressBar progressBar ; 
    std::vector<std::string_view> tags;
    for (const QString& tag : tagNames) tags.push_back(std::string_view(tag.toUtf8().data()));
    
    const auto res = dbase->stripGames(HFilter(&filter_all), progressBar, tags);
    
    QList<unsigned short> l;
    l<<res.first;
    l<<res.second;
    return l;
}

errorT ChessBase::tagList(scidBaseT *dbase, QList<QPair<QString,int>> &res)
{
    std::map<std::string, gamenumT, std::less<>> tag_freq;  
    ProgressBar progress;
    for (gamenumT gnum = 0, n = dbase->numGames(); gnum < n; ++gnum) {
        if ((gnum % 1024 == 0) && !progress.report(gnum, n))
            return  ERROR_UserCancel;
        
        const auto ie = dbase->getIndexEntry(gnum);
        const auto err = dbase->getGame(*ie).decodeTags(
            [&](auto const& tag, auto const&) {
                auto it = tag_freq.find(tag);
                if (it == tag_freq.end())
                    tag_freq.emplace(tag, 1);
                else
                    it->second++;
            });
        if (err)
            return err;
    }   
    for (const auto& [tag, freq] : tag_freq) {
		if (tag != "SetUp") {
		   QPair <QString,int> p;
		   p.first=tag.c_str();
		   p.second=freq;
		   res<<p;
		}
	}
	return OK;
    
}


errorT ChessBase::playerElo(const scidBaseT *dbase, QString filterName, QString playerName, QList<QPair<unsigned , ushort> > &res)
{
    const HFilter filter = dbase->getFilter(filterName.toStdString().c_str());
    if ( filter== nullptr ) return ERROR_BadArg;
    
    idNumberT id = 0;
	if (dbase->getNameBase()->FindExactName(NAME_PLAYER, playerName.toStdString().c_str(), &id) != OK)
		return  OK;
		
    QMap<unsigned, eloT> eloByMonth;
	auto getPlayerElo = [](auto idx_entry, auto player_id) -> eloT {
		if (idx_entry->GetWhite() == player_id)
			return idx_entry->GetWhiteElo();
		if (idx_entry->GetBlack() == player_id)
			return idx_entry->GetBlackElo();
		return 0;
	};
	for (auto gnum : filter) {
		const IndexEntry* ie = dbase->getIndexEntry(gnum);
		if (auto elo = getPlayerElo(ie, id)) {
			auto date = ie->GetDate();
			auto year = date_GetYear(date);
			auto month = date_GetMonth(date);
			if (month > 0) {
				month--;
			}
			if (month > 11) {
				month = 0;
			}
			eloByMonth[year * 12 + month] = elo;
		}
	}

	
	for (auto& e : eloByMonth.keys()) {
         QPair<unsigned ,int> p(e/12.0,eloByMonth[e]);
		res.push_back(p);       // elo
	}
	return OK;
}


bool ChessBase::baseInUse(  int number)
{
    auto baseptr=DBasePool::getBase(number);
    return (baseptr!=0 );
}

int ChessBase::exportBase(const scidBaseT *dbase, QString filter, bool appendToFile, gameFormatT outputFormat, QString exportFileName, 
                           QString startText, QString endText, bool comments, bool variation, bool spaces, bool symbols,
                           bool indentComments, bool indentVariation, bool column, bool noMarkCodes, bool convertNullMoves) 
{
    bool exportFilter = ( filter != QString("current")) ;
    if (exportFilter  &&  !dbase->inUse) {
        return  ERROR_FileNotOpen;
    }
    uint pgnStyle=PGN_STYLE_TAGS;
    if (comments)         pgnStyle |= PGN_STYLE_COMMENTS ;
    if (variation)        pgnStyle |= PGN_STYLE_VARS;
    if (spaces)           pgnStyle |= PGN_STYLE_MOVENUM_SPACE;
    if (symbols)          pgnStyle |= PGN_STYLE_SYMBOLS;
    if (indentComments)   pgnStyle |= PGN_STYLE_INDENT_COMMENTS; 
    if (indentVariation)  pgnStyle |= PGN_STYLE_INDENT_VARS; 
    if (column)           pgnStyle |= PGN_STYLE_COLUMN; 
    if (noMarkCodes)      pgnStyle |= PGN_STYLE_STRIP_MARKS;
    if (convertNullMoves) pgnStyle |= PGN_STYLE_NO_NULL_MOVES; 
    
    FILE * exportFile = NULL;
    exportFile = fopen (exportFileName.toStdString().c_str(), (appendToFile ? "r+" : "w"));
    if (exportFile == NULL) {
        return ERROR_FileOpen;
    }
    // Write start text or find the place in the file to append games:
    if (appendToFile) {
        if (outputFormat == PGN_FORMAT_Plain) {
            fseek (exportFile, 0, SEEK_END);
        } else {
            fseek (exportFile, 0, SEEK_SET);
            const char * endMarker = "";
            if (outputFormat == PGN_FORMAT_HTML) {
                endMarker = "</body>";
            } else if (outputFormat == PGN_FORMAT_LaTeX) {
                endMarker = "\\end{document}";
            }
            char line [1024];
            uint pos = 0;
            while (1) {
                char* err = fgets(line, 1024, exportFile);
                if (err == 0 || feof(exportFile)) break;
                const char * s = strTrimLeft (line, " ");
                if (strIsCasePrefix (endMarker, s)) {
                    // We have seen the line to stop at, so break out
                    break;
                }
                pos = ftell (exportFile);
            }
            fseek (exportFile, pos, SEEK_SET);
        }
    } else {
        QByteArray byteArray = startText.toUtf8();
        const char* cString = byteArray.constData();
        fputs (cString, exportFile);
    }

    if (!exportFilter) {
        exportGame (dbase->game, exportFile, outputFormat, pgnStyle);
    } else { //TODO: remove this (duplicate of sc_filter export)
        Progress progress ;
        uint numSeen = 0;
        uint numToExport = dbase->dbFilter->Count();
        Game * g = scratchGame;
        for (gamenumT i=0, n=dbase->numGames(); i < n; i++) {
            if (dbase->dbFilter->Get(i)) { // Export this game:
                if (++numSeen % 1024 == 0) {  // Update the percentage done bar:
                    if (!progress.report(numSeen, numToExport)) break;
                }

                // Print the game, skipping any corrupt games:
                const IndexEntry* ie = dbase->getIndexEntry(i);
                if (ie->GetLength() == 0) { continue; }
                if (dbase->getGame(*ie, *g) != OK) {
                    continue;
                }
                exportGame (g, exportFile, outputFormat, pgnStyle);
            }
        }
        progress.report(1, 1);
    }
    QByteArray byteArray = endText.toUtf8();
     const char* cString = byteArray.constData();
    fputs (cString, exportFile);
    fclose (exportFile);
    return OK;
}

void ChessBase::exportGame(Game *g, FILE *exportFile, gameFormatT format, uint pgnStyle )
{
    
    char old_language = language;

    g->ResetPgnStyle (pgnStyle);
    g->SetPgnFormat (format);

    // Format-specific settings:
    switch (format) {
    case PGN_FORMAT_HTML:
    case PGN_FORMAT_LaTeX:
        g->AddPgnStyle (PGN_STYLE_SHORT_HEADER);
        break;
    default:
        language = 0;
        break;
    }
    static uint htmlDiagStyle = 0;
    g->SetHtmlStyle (htmlDiagStyle);
    std::pair<const char*, unsigned> pgn = g->WriteToPGN(75, true, format != PGN_FORMAT_LaTeX);
    //size_t nWrited =
    fwrite(pgn.first, 1, pgn.second, exportFile);
    //TODO:
    //if (nWrited != db->tbuf->GetByteCount()) error
    language = old_language;
}

bool ChessBase::checkDuplicate(scidBaseT *dbase, const IndexEntry *ie1, const IndexEntry *ie2, dupCriteriaT *cr)
{
     if (ie1->GetDeleteFlag()  ||  ie2->GetDeleteFlag()) { return false; }
    if (cr->sameEvent) {
        if (ie1->GetEvent() != ie2->GetEvent()) { return false; }
    }
    if (cr->sameSite) {
        if (ie1->GetSite() != ie2->GetSite()) { return false; }
    }
    if (cr->sameRound) {
        if (ie1->GetRound() != ie2->GetRound()) { return false; }
    }
    if (cr->sameYear) {
        if (ie1->GetYear() != ie2->GetYear()) { return false; }
    }
    if (cr->sameMonth) {
        if (ie1->GetMonth() != ie2->GetMonth()) { return false; }
    }
    if (cr->sameDay) {
        if (ie1->GetDay() != ie2->GetDay()) { return false; }
    }
    if (cr->sameResult) {
        if (ie1->GetResult() != ie2->GetResult()) { return false; }
    }
    if (cr->sameEcoCode) {
        ecoStringT a;
        ecoStringT b;
        eco_ToBasicString (ie1->GetEcoCode(), a);
        eco_ToBasicString (ie2->GetEcoCode(), b);
        if (a[0] != b[0]  ||  a[1] != b[1]  ||  a[2] != b[2]) { return false; }
    }

    // There are a lot of "place-holding" games in some database, that have
    // just one (usually wrong) move and a result, that are then replaced by
    // the full version of the game. Therefore, if we reach here and one
    // of the games (or both) have only one move or no moves, return true
    // as long as they have the same year, site and round:

    if (ie1->GetNumHalfMoves() <= 2  ||  ie2->GetNumHalfMoves() <= 2) {
        if (ie1->GetYear() == ie2->GetYear()  &&
            ie1->GetSite() == ie2->GetSite()  &&
            ie1->GetRound() == ie2->GetRound()) {
            return true;
        }
    }

    // Now check that the games contain the same moves, up to the length
    // of the shorter game:

    if (cr->sameMoves) {
        const byte * hpData1 = ie1->GetHomePawnData();
        const byte * hpData2 = ie2->GetHomePawnData();
        if (! hpSig_Prefix (hpData1, hpData2)) { return false; }
        // Now we have to check the actual moves of the games:
        uint length = std::min(ie1->GetNumHalfMoves(), ie2->GetNumHalfMoves());
        std::string a = dbase->getGame(ie1).getMoveSAN(0, length);
        std::string b = dbase->getGame(ie2).getMoveSAN(0, length);
        return (a == b);
    }
    return true;
}


errorT ChessBase::duplicates(scidBaseT *dbase, bool players, bool colors, bool event, bool site,
                             bool round, bool year, bool month, bool day, bool result, bool eco,
                             bool moves, bool skipShortGames, bool keepAllCommentedGames, bool keepAllGamesWithVars, bool setFilterToDups,
                             bool onlyFilterGames, enum deleteStrategyT deleteStrategy)
{
    dupCriteriaT criteria;
    const gamenumT numGames = dbase->numGames();
    criteria.exactNames  = players;
    criteria.sameColors  = colors;
    criteria.sameEvent   = event;
    criteria.sameSite    = site;
    criteria.sameRound   = round;
    criteria.sameYear    = year;
    criteria.sameMonth   = month;
    criteria.sameDay     = day;
    criteria.sameResult  = result;
    criteria.sameEcoCode = eco;
    criteria.sameMoves = moves;
   

    // Setup duplicates array:
    auto duplicates = std::make_unique<gamenumT[]>(numGames);

    // We use a hashtable to limit duplicate game comparisons; each game
    // is only compared to others that hash to the same value.
    std::vector<gNumListT> hash(numGames);
    size_t n_hash = 0;
    const std::vector<uint32_t>& hashMap = (criteria.exactNames)
            ? std::vector<uint32_t>()
            : dbase->getNameBase()->generateHashMap(NAME_PLAYER);
    for (gamenumT i=0; i < numGames; i++) {
        const IndexEntry* ie = dbase->getIndexEntry(i);
        if (! ie->GetDeleteFlag()  /* &&  !ie->GetStartFlag() */
            &&  (!skipShortGames  ||  ie->GetNumHalfMoves() >= 10)
            &&  (!onlyFilterGames  ||  dbase->dbFilter->Get(i) > 0)) {

            uint32_t wh = ie->GetWhite();
            uint32_t bl = ie->GetBlack();
            if (!criteria.exactNames) {
                wh = hashMap[wh];
                bl = hashMap[bl];
            }
            if (!criteria.sameColors && bl > wh) {
                std::swap(wh, bl);
            }
            gNumListT* node = &(hash[n_hash++]);
            node->hash = (uint64_t(wh) << 32) + bl;
            node->gNumber = i;
        }
    }
    hash.resize(n_hash);
    std::sort(hash.begin(), hash.end());

    Filter tmp_filter(numGames);
    HFilter filter = setFilterToDups ? dbase->getFilter("dbfilter")
                                     : HFilter(&tmp_filter);
    filter.clear();
    Progress progress ;
    // Now check same-hash games for duplicates:
    for (size_t i=0; i < n_hash; i++) {
        if ((i % 1024) == 0) {
            if (!progress.report(i, numGames)) break;
        }
        const gNumListT& head = hash[i];
        const IndexEntry* ieHead = dbase->getIndexEntry(head.gNumber);

        for (size_t comp=i+1; comp < n_hash; comp++) {
            const gNumListT& compare = hash[comp];
            if (compare.hash != head.hash) break;

            const IndexEntry* ieComp = dbase->getIndexEntry(compare.gNumber);

            if (checkDuplicate(dbase, ieHead, ieComp, &criteria)) {
                duplicates[head.gNumber] = compare.gNumber + 1;
                duplicates[compare.gNumber] = head.gNumber + 1;

                auto isImmune = [&](const IndexEntry* ie) {
                    if (keepAllCommentedGames && ie->GetCommentsFlag())
                        return true;
                    return keepAllGamesWithVars && ie->GetVariationsFlag();
                };

                // Decide which game should get deleted:
                bool deleteHead = false;
                if (deleteStrategy == DELETE_OLDER) {
                    deleteHead = (head.gNumber < compare.gNumber);
                } else if (deleteStrategy == DELETE_NEWER) {
                    deleteHead = (head.gNumber > compare.gNumber);
                } else {
                    ASSERT(deleteStrategy == DELETE_SHORTER);
                    uint a = ieHead->GetNumHalfMoves();
                    uint b = ieComp->GetNumHalfMoves();
                    deleteHead = (a <= b);
                    if (a == b && isImmune(ieHead))
                        deleteHead = false;
                }

                gamenumT gnumDelete = compare.gNumber;
                const IndexEntry* ieDelete = ieComp;
                if (deleteHead) {
                    gnumDelete = head.gNumber;
                    ieDelete = ieHead;
                }
                // Delete whichever game is to be deleted:
                if (!isImmune(ieDelete)) {
                    filter->set(gnumDelete, 1);
                }
            }
        }
    }
    auto[err, nDel] = dbase->transformIndex(filter, {}, [](IndexEntry& ie) {
        ie.SetDeleteFlag(true);
        return true;
    });
    dbase->setDuplicates(std::move(duplicates));
    progress.report(1, 1);
    return (err == OK) ?  OK :  err;     
    
}


errorT ChessBase::gamesummary(const scidBaseT& base,uint gameNum, QList<QVariant> &res)
{
    Game* g = scratchGame;
	gamenumT gnum = gameNum;
	if (gnum > 0) {
		auto ie = base.getIndexEntry_bounds(gnum - 1);
		if (!ie || base.getGame(*ie, *scratchGame) != OK) {
			return ERROR_BadArg; 
		}
	} else {
		g = base.game;
	}


    // Return header summary if requested:
        DString dstr;
        dstr.Append (g->GetWhiteStr());
        eloT elo = g->GetWhiteElo();
        if (elo > 0) { dstr.Append (" (", elo, ")"); }
        dstr.Append ("  --  ", g->GetBlackStr());
        elo = g->GetBlackElo();
        if (elo > 0) { dstr.Append (" (", elo, ")"); }
        dstr.Append ("\n", g->GetEventStr());
        const char * round = g->GetRoundStr();
        if (! strIsUnknownName(round)) {
            dstr.Append (" (", round, ")");
        }
        dstr.Append ("  ", g->GetSiteStr(), "\n");
        char dateStr [20];
        date_DecodeToString (g->GetDate(), dateStr);
        // Remove ".??" or ".??.??" from end of date:
        if (dateStr[4] == '.'  &&  dateStr[5] == '?') { dateStr[4] = 0; }
        if (dateStr[7] == '.'  &&  dateStr[8] == '?') { dateStr[7] = 0; }
        dstr.Append (dateStr, "  ");
        dstr.Append (RESULT_LONGSTR[g->GetResult()]);
        ecoT eco = g->GetEco();
        if (eco != 0) {
            ecoStringT ecoStr;
            eco_ToExtendedString (eco, ecoStr);
            dstr.Append ("  ", ecoStr);
        }
        res.push_back(dstr.Data());

    // Here, a list of the boards or moves is requested:
    const auto n_moves = g->GetNumHalfMoves() + 1;
    QStringList boards;
    QStringList moves;
    auto location = g->currentLocation();
    g->MoveToStart();
    do {
            char boardStr[100];
            g->GetCurrentPos()->MakeLongStr (boardStr);
            boards.push_back(boardStr);

            colorT toMove = g->GetCurrentPos()->GetToMove();
            uint moveCount = g->GetCurrentPos()->GetFullMoveCount();
            char san [20];
            g->GetSAN (san);
            if (san[0] != 0) {
                char temp[40];
                if (toMove == WHITE) {
                    sprintf (temp, "%u.%s", moveCount, san);
                } else {
                    strCopy (temp, san);
                }
                byte * nags = g->GetNextNags();
                if (*nags != 0) {
                    for (uint nagCount = 0 ; nags[nagCount] != 0; nagCount++) {
                        char nagstr[20];
                        game_printNag (nags[nagCount], nagstr, true,
                                       PGN_FORMAT_Plain);
                        if (nagCount > 0  ||
                              (nagstr[0] != '!' && nagstr[0] != '?')) {
                            strAppend (temp, " ");
                        }
                        strAppend (temp, nagstr);
                    }
                }
                moves.push_back(temp);
            } else {
                moves.push_back(RESULT_LONGSTR[g->GetResult()]);
            }

    } while (g->MoveForward() == OK);
    g->restoreLocation(location);

    res.push_back(boards);
    res.push_back(moves);
    return OK;
}



errorT ChessBase::tournaments(const scidBaseT *dbase, Filter, QString filter, long maxresult, QList<QList<QVariant>> &res, int elo, int games, int players, QString player, QString sort)
{

    const HFilter hfilter= dbase->getFilter(filter.toStdString().c_str());
    if (hfilter==0) return ERROR_BadArg;
    SearchTournaments search (dbase,hfilter);
    
    QString sortCriteria;
    long nResults=maxresult ;
    if (elo!=0)   search.filterByAvgElo(StrRange(QString("%1").arg(elo).toStdString().c_str()));
    if (games!=0) search.filterByNGames(StrRange(QString("%1").arg(games).toStdString().c_str()));
    if (players!=0) search.filterByNPlayers(StrRange(QString("%1").arg(players).toStdString().c_str()));
    if ( !player.isEmpty()) search.filterByPlayer(player.toStdString().c_str());
    if ( !sort.isEmpty())   sortCriteria=sort;
    if ( !sort.isEmpty()) { if (!search.sort(sortCriteria.toStdString().c_str(),nResults)) return ERROR_BadArg;}
 
    SearchTournaments::Iter it = search.begin();
	SearchTournaments::Iter it_end = search.end();
	if (std::distance(it, it_end) > nResults) {
		it_end = it + nResults;
	}
     char buf_date[16];
	const NameBase* nb = dbase->getNameBase();
    
	QList<QVariant> tourney;
	for (; it != it_end; it++) {
		tourney.clear();
		date_DecodeToString(it->getStartDate(), buf_date);
		strTrimDate(buf_date);
		tourney.push_back(QString(buf_date));
		tourney.push_back(nb->GetName(NAME_SITE, it->getSiteId()));
		tourney.push_back(nb->GetName(NAME_EVENT, it->getEventId()));
		tourney.push_back(it->nPlayers());
		tourney.push_back(it->nGames());
		tourney.push_back(it->getAvgElo());
		tourney.push_back(it->getStartGameNum() + 1);
		const char* name1st = "";
		eloT elo1st = 0;
		double score1st = 0.0;
		const char* name2nd = "";
		eloT elo2nd = 0;
		double score2nd = 0.0;
		if (it->nPlayers() > 0) {
			const Tourney::Player& p = it->getPlayer(0);
			name1st = nb->GetName(NAME_PLAYER, p.nameId);
			elo1st = p.elo;
			score1st = p.score / 2.0;
		}
		if (it->nPlayers() > 1) {
			const Tourney::Player& p = it->getPlayer(1);
			name2nd = nb->GetName(NAME_PLAYER, p.nameId);
			elo2nd = p.elo;
			score2nd = p.score / 2.0;
		}
		tourney.push_back(name1st);
		tourney.push_back(elo1st);
		tourney.push_back(score1st);
		tourney.push_back(name2nd);
		tourney.push_back(elo2nd);
		tourney.push_back(score2nd);
		res.push_back(tourney);
	}
     
     
     return OK;
}





uint ChessBase::open(scidBaseT *dbase,ICodecDatabase::Codec codec, QString filename  )  
{
    return DBasePool::find(filename.toStdString().c_str());
}

uint ChessBase::numberGames(scidBaseT *dbase)
{
    return dbase->numGames();
}



errorT ChessBase::piecetrack ( scidBaseT *dbase , bool timeOnSquareMode ,uint minMoves, uint maxMoves, QString moves ,  std::array<unsigned int, 64>& arr)
{
    uint minPly = minMoves*2;
    uint maxPly = maxMoves*2;

    // Convert moves to plycounts, e.g. "5-10" -> "9-20"
    if (minPly < 2) { minPly=2; }
    if (maxPly < minPly) { maxPly = minPly; }
    minPly--;

    QStringList movelist=moves.split(' ');
    // Parse the variable number of tracked square arguments:
    uint sqFreq[64] = {0};
    bool trackSquare[64] = { false };
    int nTrackSquares = 0;
    for (auto a: movelist) {
        squareT sq = strGetSquare (a.toStdString().c_str());
        if (sq == NULL_SQUARE) { return ERROR_BadArg; }
        if (!trackSquare[sq]) {
            // Seen another starting square to track.
            trackSquare[sq] = true;
            nTrackSquares++;
        }
    }

    // If current base is unused, filter is empty, or no track
    // squares specified, then just return a zero-filled list:

    if (! dbase->inUse  ||  dbase->dbFilter->Count() == 0  ||  nTrackSquares == 0) {
        for (uint i=0; i < 64; i++) { arr[i]=0; }
        return OK;
    }

    // Examine every filter game and track the selected pieces:

    Progress progress ;
    const auto filter = dbase->getFilter("dbfilter");
    const size_t filterCount = filter->size();
    size_t filterSeen = 0;
    for (const auto gnum : filter) {
        if (++filterSeen % 1024 == 0) {
            if (!progress.report(filterSeen, filterCount)) {
                return  ERROR_UserCancel;
            }
        }

        const IndexEntry* ie = dbase->getIndexEntry(gnum);

        // Skip games with non-standard start or no moves:
        if (ie->GetStartFlag()) { continue; }
        if (ie->GetLength() == 0) { continue; }

        // Skip games too short to be useful:
        if (ie->GetNumHalfMoves() < minPly) { continue; }

        // Set up piece tracking for this game:
        bool movedTo[64] = { false };
        bool track[64];
        int ntrack = nTrackSquares;
        for (uint sq=0; sq < 64; sq++) { track[sq] = trackSquare[sq]; }

        // Process each game move until the maximum ply or end of
        // the game is reached:
        uint plyCount = 0;
        dbase->getGame(ie).mainLine([&](auto move) {
            if (plyCount++ >= maxPly)
                return false;

            squareT toSquare = move.getTo();
            squareT fromSquare = move.getFrom();

            // Special hack for castling:
            if (move.isCastle()) {
                if (toSquare == H1) {
                    if (track[H1]) {
                        fromSquare = H1;
                        toSquare = F1;
                    } else {
                        toSquare = G1;
                    }
                } else if (toSquare == A1) {
                    if (track[A1]) {
                        fromSquare = A1;
                        toSquare = D1;
                    } else {
                        toSquare = C1;
                    }
                } else if (toSquare == H8) {
                    if (track[H8]) {
                        fromSquare = H8;
                        toSquare = F8;
                    } else {
                        toSquare = G8;
                    }
                } else if (toSquare == A8) {
                    if (track[A8]) {
                        fromSquare = A8;
                        toSquare = D8;
                    } else {
                        toSquare = C8;
                    }
                }
            }

            // TODO: Special hack for en-passant capture?

            if (track[toSquare]) {
                // A tracked piece has been captured:
                track[toSquare] = false;
                ntrack--;
                if (ntrack <= 0)
                    return false;

            } else if (track[fromSquare]) {
                // A tracked piece is moving:
                track[fromSquare] = false;
                track[toSquare] = true;
                if (plyCount >= minPly) {
                    // If not time-on-square mode, and this
                    // new target square has not been moved to
                    // already by a tracked piece in this game,
                    // increase its frequency now:
                    if (!timeOnSquareMode  && !movedTo[toSquare]) {
                        sqFreq[toSquare]++;
                    }
                    movedTo[toSquare] = true;
                }
            }

            if (timeOnSquareMode  &&  plyCount >= minPly) {
                // Time-on-square mode: find all tracked squares
                // (there are ntrack of them) and increment the
                // frequency of each.
                int nleft = ntrack;
                for (uint i=0; i < 64; i++) {
                    if (track[i]) {
                        sqFreq[i]++;
                        nleft--;
                        // We can stop early when all tracked
                        // squares have been found:
                        if (nleft <= 0)
                            return false;
                    }
                }
            }

            return true;
        }); // while (plyCount < maxPly)
    } // foreach game

    progress.report(1, 1);

    // Now return the 64-integer list: if in time-on-square mode,
    // the value for each square is the number of plies when a
    // tracked piece was on it, so halve it to convert to moves:

    for (uint i=0; i < 64; i++) {
        arr[i]= timeOnSquareMode ? sqFreq[i] / 2 : sqFreq[i];
    }
    return OK;
}


