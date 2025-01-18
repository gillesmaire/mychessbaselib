
# Scid5lib Developper guide


## Scid5Lib Presentation

###  The goal of scid5lib 

Scid5lib is a port of Scid database management from Scid program 
https://scid.sourceforge.net/ onto Qt6/C++ environment

The goal is to use Scid5 database for Qt6 programs. 

Scid5Lib is only database management and this guide explains how 
to use the library, it doesn't concern the chessboard but only the
storage of Games on disk.

You can find all the library fonctions in the  chessbase.h file plus 
some other functionality described in some other files.

You can call doxygen on this file to get a documentation in HTML format

### The Goal of this document

This document is a development user guide to explain how to use the library.

### Where you can find this document 

https://github.com/gillesmaire/mychessbaselib/blob/main/doc/devguide.md


### Where you can find this project 

https://github.com/gillesmaire/mychessbaselib

### What is the difference between myChessBase and scid5lib

myChessBase is an other project to write a Chess base management 

myChessBase use scid5lib to manage base

I you use this document it is to develop an other one program like myChessBase 

### About the author 

Gilles Maire writing https://www.gillesmaire.com

I am 63 years old man specialist Qt and embeded Linux living in Paris. You can
find all my developpements on https://www.gillesmaire.com

You can contact me for job à gilles@gillesmaire.com

## scid5lib developper guide 

### Exemple environment

The file to learn and test the scid5lib is included in the
following files : 

- Examples.pro 
- main.cpp 
- mainwindow.ui
- mainwindow.cpp
- mainwindow.h

### Create a new base

- You have to call the init method from  DBaseSpool. 
- You have to call ChessBase cb constructor from  mainWindow of from a widget
- You call create method 

#### The class ChessBase

The ChessBase is the class containing all the method to manipulate database
You can find the list of methods in ChessBase.h and get the dictionary
running the doxygen file on this file.

#### Localization of DataBase 

The following line create a database in the directory where program is called

~~~cpp
    int code=cb.create("mydatabase",ICodecDatabase::SCID5,numberbase);
~~~

If code is equal to OK all is good, else you get a error code defined in error.g file

You can give a path like /home/gilles/Database whith the line 
You have to choose the format of database between :  ICodecDatabase::SCID5,
ICodecDatabase::SCID4 (old format), ICodecDatabase::PGN (if you want a slow to open but
human readable database)
 
The scid3 format is not supported, and SCID5 format is the more efficient

~~~cpp
    cb.create("/home/gilles/Database/mydatabase",ICodecDatabase::SCID5,numberbase);
~~~

If you  can also can open a PGN file with ICodecDatabase::PGN. In this case
you have to add pgn extention 

~~~cpp
    cb.create("/home/gilles/Database/mydatabase.pgn",ICodecDatabase::PGN,numberbase);
~~~



### DBaseSpool concept 

**DBaseSpool* allow to manipulate several Data>
bases and switch from on one another one.
The description is made in the dbasespool.h file.

The set of Databases is named a pool. This pool need to be initialized at the 
begining of the program by the **Init** method.
This initialization must be before MainWindow initialization, for example 
between Application initialization  and  mainWindow Initialization

~~~cpp
QApplication a(argc, argv);
DBasePool::init()  ;
MainWindow mainWindow; 
~~~


The **getHandles()** function allows to initalize a vector with all opened databases plus the clipbase


~~~cpp
auto vec = DBasePool::getHandles() ;
qDebug()<<"nb bases"<<vec.size() 
~~~


Each databases are done by the vec variable.
If no database is opened, the number of bases will be 1 for the clipbase.

You can ask user to choose the database in a menu. 

The maximum number of databases is 9.

The clip base can me reinitilized by : clearClipBase

The funtion closeAll will close the database and save them and
they are flushed on the disk.

The find function will give the handler from
the name of filename.  The complete path
must be done with the extension si5 ou si4.

For example if your database is SlavMain
you have to  call find("/home/gilles/scid5listTest/SlavMain.si5")

Be care to don't call with "/home/gilles/scid5listTest/SlavMain.sn5"
or "/home/gilles/scid5listTest/SlavMain.sn5"
or "/home/gilles/scid5listTest/SlavMain"
or "/home/gilles/scid5listTest//SlavMain.si5"

It is more safe to keep the Handles and to call scidBaseT* getBase(int baseHandle) than 
try to retriev if from find function. 




~~~cpp
int DBasePool::find (cont char *filename)
~~~

When number of basespool is know the scidbase *
pointer can be set by the function

~~~cpp
scidbaseT *getBase(int basehandle)
~~~


To retrieve all the opened databases getHandles 


~~~cpp
std::vector<int> getHandles();
~~~

vector is something like : 1, 2, 9  where 1  is handle for clipbase,

2 is the handle of first base, 9 the handle for second base etc    

With the handle you can get scidbaseT * with getBase(handle);


### Import  PNG file 

To import a PGN File, you must create for example a scid5 base 
and the import the PGN file. 

You can read a PGN File directly but the next time you will open the PGN 
file you will loose time to decode it. 

You know how to create a scid5 base with open.

This is an example to import PGN games in scid5 database 


~~~cpp
#include <QApplication>
#include "game.h"
#include "chessbase.h"
#include <QMainWindow>
#include <dbasepool.h>
#include <QDebug>


static Game * scratchGame = NULL;   

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow mainWindow; 
    mainWindow.setWindowTitle("Application Qt Minimal");
    mainWindow.resize(800, 600); 
    mainWindow.show();
    DBasePool::init()  ;
    ChessBase cb(&mainWindow);
    int number;
    int code=cb.open("/home/gilles/Test/test",ICodecDatabase::SCID5,FMODE_Create,number);
    qDebug()<<code<<number;
    if ( code != OK ) return a.exec();
    scidBaseT *dbase =DBasePool::getBase(number);
    code=cb.importGames(dbase,"/home/gilles/Developpements/PGN/00003265parties.pgn",number);
    return a.exec();
}
~~~

## WIP

### Filter and HFilter concept 

#### Filter 

It is possible to search according different criteria.
But for all of theses creteria, the list of matching games is stored in Filter Object.

- 0 value indicates the game is excluded
- 1-255 indicates the game is included and the position 
to show when the game is loaded. 
	- 1 is the start position
	- 2 is the next one ie the position after the 
	firt white move. 
	- 3 is tne position aftet the black first move
	
	
