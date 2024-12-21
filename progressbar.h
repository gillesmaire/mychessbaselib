#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H
#include <QProgressBar>

#include "misc.h"

class ProgressBar : public Progress
{
public:
    ProgressBar();
    void  setValues( int done, int total) ;
    void setTotal(int total);
    void setDone( int done);
private:
   QProgressBar * mProgressBar;
};

#endif // PROGRESSBAR_H
