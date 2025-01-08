TEMPLATE = lib
CONFIG += sharedlib
TARGET = QtScid

CONFIG += c++20

QT  += core gui widgets
SOURCES += \
         chessbase.cpp \
         scidbase.cpp \
         matsig.cpp \
         misc.cpp \
         game.cpp \
         position.cpp \
         codec_scid4.cpp \
         sortcache.cpp \
         stored.cpp \
         textbuf.cpp \
         dbasepool.cpp

HEADERS += scidbase.h \
 		bytebuf.h \
 		board_def.h \
 		chessbase.h \
 		codec.h \  \
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
 		searchtournaments.h
		
