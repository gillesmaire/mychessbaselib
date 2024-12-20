#ifndef CHESSBASE_H
#define CHESSBASE_H
#include <QObject>
#include "common.h"
#include "error.h"

class ChessBase : public QObject
{
public:
    ChessBase( QObject *ob);
    errorT close();
    /// \brief open open a base with value of codec  "MEMORY", "PGN", "SCID4","SCID5"
    /// \param codec : can be "MEMORY", "PGN", "SCID4","SCID5"
    /// \param fmode : can be member of enum FMODE_None = 0,FMODE_ReadOnly,FMODE_WriteOnly,FMODE_Both,FMODE_Create
    /// \param filename : name of filename 
    /// \param numberbase : a return parameter indicate the number of base selected
    /// \return : return 0 if no error else a value given in error.h
    errorT open( QString codec, fileModeT fmode ,QString filename, int  &numberbase);
};

#endif // CHESSBASE_H
