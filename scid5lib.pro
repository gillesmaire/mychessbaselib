QT = core gui widgets

CONFIG += c++20 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        chessbase.cpp \
        main.cpp \
        scidbase.cpp \
        matsig.cpp \
        misc.cpp \
        game.cpp \
        position.cpp \
        codec_scid4.cpp \
        sortcache.cpp \
        stored.cpp \
        textbuf.cpp \
        dbasepool.cpp \
        progressbar.cpp

HEADERS += scidbase.h \
		bytebuf.h \
		board_def.h \
		chessbase.h \
		codec.h \ 
		error.h \
		common.h \
		namebase.h \
		containers.h \
		indexentry.h \
		date.h \
		matsig.h\
		misc.h  \
		game.h \
		movetree.h \
		movelist.h  \
		position.h \ 
		gameview.h \
		fullmove.h \
		movegen.h \
		tree.h \
		hfilter.h \
		codec_memory.h \
		codec_pgn.h \
		codec_proxy.h \
		filebuf.h \
		pgn_encode.h \
		pgnparse.h \
		pgn_lexer.h \
		codec_scid4.h \
		codec_scid5.h \
		sortcache.h \
		stored.h \
		attacks.h \
		dstring.h \
		hash.h \
		sqmove.h \
		nagtext.h \
		naglatex.h \
		textbuf.h \
		dbasepool.h \
		progressbar.h
		
		
		
	

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
