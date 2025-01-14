QT = core gui widgets

CONFIG += c++20 cmdline

OTHER_FILES += doc/devguide.md	

LIBS += -L../lib -lQtScid

INCLUDEPATH += $$PWD/../lib/
DEPENDPATH += $$PWD/../lib/

SOURCES += \
		main.cpp \
		mainwindow.cpp \
		scidbasemodel.cpp
		
HEADERS +=  \
		mainwindow.h \
		scidbasemodel.h

FORMS += \
     mainwindow.ui

