#include "progressbar.h"

ProgressBar::ProgressBar() {
mProgressBar = new QProgressBar;
}

void ProgressBar::setValues(int done, int total)
{
    setTotal(total);
    setDone(done);
}

void ProgressBar::setTotal(int total)
{
  mProgressBar->setMaximum(total);
}

void ProgressBar::setDone(int done)
{
    mProgressBar->setValue(done);
}
