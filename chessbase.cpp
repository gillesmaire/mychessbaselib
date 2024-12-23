#include "chessbase.h"
#include "dbasepool.h"
#include "scidbase.h"
#include "progressbar.h"
#include "misc.h"

#include <QList>
#include <QPair>

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

