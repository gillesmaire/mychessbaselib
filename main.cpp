#include <QCoreApplication>
#include "game.h"

static Game * scratchGame = NULL;   

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // this code will be deleted when library will be coded

    return a.exec();
}
