
# Scid5lib Developper guide


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


#### DataBase creation 

#### Create a new base

- You have to call the init method from  DBasePool. 
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















