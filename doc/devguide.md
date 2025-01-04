
# Scid5lib Developper guide


## Scid5Lib Presentation

###  The goal of scid5lib 

Scid5lib is a port of Scid database management from Scid program 
https://scid.sourceforge.net/ onto Qt6/C++ environment

The goal is to use Scid5 database for Qt6 programs. 

Scid5Lib is only database management and this guide explains how 
to use the library. 

You can find all the library fonctions  in chessbase.h 

You can call doxygen on this file to get a documentation in HTML format

### The Goal of this document

This document is a development user guide to explain how to use the library.

### Where you can find this document 

https://github.com/gillesmaire/mychessbaselib/blob/main/doc/devguide.md


### Where you can find this project 

https://github.com/gillesmaire/mychessbaselib

### What is the difference between myChessBase and scid5lib

myChessBase is an other project to write a Chess base management 

my ChessBase use scid5lib to manage base

I you use this document it is to develop an other one program like myChessBase 

### About the author 

Gilles Maire writing https://www.gillesmaire.com

I am 63 years old man specialist Qt and embeded Linux living in Paris. You can
find all my developpements on https://www.gillesmaire.com

You can contact me for job à gilles@gillesmaire.com

## scid5lib developper guide 


#### Create a new base

- You have to call the init method from  DBaseSpool. 
- You have to call ChessBase cb constructor from  mainWindow of from a widget
- You cal create method 
 
~~~cpp
#include <QApplication>
#include "game.h"
#include "chessbase.h"
#include <QMainWindow>
#include <dbasepool.h>

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
    int numberbase;
    cb.create("mydatabase",ICodecDatabase::SCID5,numberbase);
    return a.exec();
}

~~~~

#### Localization of DataBase 

The following line create a database in the directory where program is called

~~~cpp
    cb.create("mydatabase",ICodecDatabase::SCID5,numberbase);
~~~

You can give a path like /home/gilles/Database whith the line 

~~~cpp
    cb.create("/home/gilles/Database/mydatabase",ICodecDatabase::SCID5,numberbase);
~~~

You can choose ICodecDatabase::SCID4 but it is less efficient

You  can also can open a PGN file with ICodecDatabase::PGN. In this case
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

Severall Databases can be close with **spool.closeAll()**

It is possible to get an Handle corresponding to a filename by the **find** function
returning an integer. With this integer we can't get the scidBaseT *ptr but the function
**getBase()** can do it.  The scidBase is used in all the chessbase class methods. 

DBaseSpool allows to call **getClipBase()**  a special memory database that is always open and valid.

**scidBaseT* getFreeSlot()** allocate a free database spool (or nullptr) 
all slot are busy.


To retrieve all the opened databases **getHandles return a std::vector<int>**


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
    scidBaseT *dbase =DBasePool::getBase(number);
    code=cb.importGames(dbase,"/home/gilles/Developpements/PGN/00003265parties.pgn",number);
    return a.exec();
}
~~~



