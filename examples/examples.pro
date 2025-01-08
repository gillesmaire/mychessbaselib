QT = core gui widgets

CONFIG += c++20 cmdline

OTHER_FILES += doc/devguide.md	

LIBS += -L$$PWD/../lib -lQtScid


INCLUDEPATH += $$PWD/../lib/
DEPENDPATH += $$PWD/../lib/

SOURCES += \
		main.cpp \
		mainwindow.cpp
		
HEADERS +=  \
		mainwindow.h

FORMS += \
     mainwindow.ui

